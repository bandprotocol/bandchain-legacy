#include "handler.h"

#include <boost/scope_exit.hpp>

#include "band/msg.h"
#include "crypto/ed25519.h"
#include "store/account.h"
#include "store/contract.h"
#include "store/revenue.h"

Handler::Handler(Context& _ctx)
    : ctx(_ctx)
{
}

void Handler::init(const std::string& init_state)
{
  ctx.create<Revenue>(
      IBAN<IBANType::Revenue>("RX70 RRRR RRRR RRRR RRRR RRRR RRRR RRRR RRRR")
          .as_addr(),
      IBAN<IBANType::Contract>("CX09 CCCC CCCC CCCC CCCC CCCC CCCC CCCC CCCC")
          .as_addr(),
      IBAN<IBANType::Account>("AX72 AAAA AAAA AAAA AAAA AAAA AAAA AAAA AAAA")
          .as_addr(),
      TimePeriod(),
      IBAN<IBANType::Stake>("SX87 SSSS SSSS SSSS SSSS SSSS SSSS SSSS SSSS")
          .as_addr(),
      false);
  ctx.create<Contract>(
      IBAN<IBANType::Contract>("CX09 CCCC CCCC CCCC CCCC CCCC CCCC CCCC CCCC")
          .as_addr(),
      IBAN<IBANType::Revenue>("RX70 RRRR RRRR RRRR RRRR RRRR RRRR RRRR RRRR")
          .as_addr(),
      IBAN<IBANType::Contract>("CX09 CCCC CCCC CCCC CCCC CCCC CCCC CCCC CCCC")
          .as_addr(),
      Curve(std::make_unique<EqVar>()), Curve(std::make_unique<EqConstant>(0)),
      std::numeric_limits<uint256_t>::max(), true, false,
      IBAN<IBANType::Account>("AX72 AAAA AAAA AAAA AAAA AAAA AAAA AAAA AAAA")
          .as_addr());
  ctx.flush();
}

void Handler::apply_message(const MsgHdr& hdr, Buffer& buf, const Hash& tx_hash,
                            int64_t timestamp)
{
  BOOST_SCOPE_EXIT(&ctx) { ctx.reset(); }
  BOOST_SCOPE_EXIT_END

  // Convert the verify key to the sender's account ID
  const AccountID account_id = AccountID::from_addr(ed25519_vk_to_addr(hdr.vk));

  switch (hdr.msgid) {
    case +MsgID::Mint:
      apply_mint(account_id, buf.read_all<MintMsg>(), tx_hash, timestamp);
      break;
    case +MsgID::Tx:
      apply_tx(account_id, buf.read_all<TxMsg>(), tx_hash, timestamp);
      break;
    case +MsgID::CreateContract:
      apply_create_contract(account_id, buf.read_all<CreateContractMsg>(),
                            tx_hash, timestamp);
      break;
    case +MsgID::PurchaseContract:
      apply_purchase_contract(account_id, buf.read_all<PurchaseContractMsg>(),
                              tx_hash, timestamp);
      break;
    case +MsgID::SellContract:
      apply_sell_contract(account_id, buf.read_all<SellContractMsg>(), tx_hash,
                          timestamp);
      break;
    case +MsgID::SpendToken:
      apply_spend_token(account_id, buf.read_all<SpendTokenMsg>(), tx_hash,
                        timestamp);
      break;
    case +MsgID::CreateRevenue:
      apply_create_revenue(account_id, buf.read_all<CreateRevenueMsg>(),
                           tx_hash, timestamp);
      break;

    case +MsgID::Unset:
      throw Error("Invalid MsgID");
  }

  ctx.flush();
}

void Handler::apply_mint(const AccountID& account_id, const MintMsg& mint_msg,
                         const Hash& tx_hash, int64_t timestamp)
{
  auto& account = ctx.get_or_create<Account>(account_id);
  account[mint_msg.token_key] += mint_msg.value;
}

void Handler::apply_tx(const AccountID& account_id, const TxMsg& tx_msg,
                       const Hash& tx_hash, int64_t timestamp)
{
  auto& account_src = ctx.get<Account>(account_id);
  auto& account_dst = ctx.get_or_create<Account>(tx_msg.dest);
  account_src[tx_msg.token_key] -= tx_msg.value;
  account_dst[tx_msg.token_key] += tx_msg.value;
}

void Handler::apply_create_contract(const AccountID& account_id,
                                    const CreateContractMsg& cc_msg,
                                    const Hash& tx_hash, int64_t timestamp)
{
  ContractID contractID = ContractID::from_addr(ed25519_vk_to_addr(tx_hash));
  auto& revenue = ctx.get<Revenue>(cc_msg.revenue_id);

  if (revenue.is_private && account_id != revenue.manager)
    throw Error(
        "Your account_idess doesn't match manager's revenue account_idess.");

  ctx.create<Contract>(contractID, cc_msg.revenue_id, revenue.base_token_id,
                       cc_msg.buy_curve, cc_msg.sell_curve, cc_msg.max_supply,
                       cc_msg.is_transferable, cc_msg.is_discountable,
                       cc_msg.beneficiary);
}

void Handler::apply_purchase_contract(const AccountID& account_id,
                                      const PurchaseContractMsg& pct_msg,
                                      const Hash& tx_hash, int64_t timestamp)
{
  auto& account = ctx.get<Account>(account_id);
  auto& contract = ctx.get<Contract>(pct_msg.contract_id);
  auto& beneficiary = ctx.get<Account>(contract.beneficiary);

  if (contract.max_supply - contract.total_supply < pct_msg.value) {
    throw Error("There aren't tokens enough for sell");
  }

  const uint256_t buy_price =
      contract.get_buy_price(contract.circulating_supply + pct_msg.value) -
      contract.get_buy_price(contract.circulating_supply);

  const uint256_t sell_price =
      contract.get_sell_price(contract.circulating_supply + pct_msg.value) -
      contract.get_sell_price(contract.circulating_supply);

  if (buy_price > pct_msg.price_limit) {
    throw Error("Invalid buy price limit {} greater than buy price",
                pct_msg.price_limit, buy_price);
  }

  account[contract.base_contract_id] -= buy_price;
  account[pct_msg.contract_id] += pct_msg.value;
  beneficiary[contract.base_contract_id] += (buy_price - sell_price);

  contract.circulating_supply += pct_msg.value;
  contract.total_supply += pct_msg.value;
}

void Handler::apply_sell_contract(const AccountID& account_id,
                                  const SellContractMsg& sct_msg,
                                  const Hash& tx_hash, int64_t timestamp)
{
  auto& account = ctx.get<Account>(account_id);
  auto& contract = ctx.get<Contract>(sct_msg.contract_id);

  const uint256_t sell_price =
      contract.get_sell_price(contract.circulating_supply) -
      contract.get_sell_price(contract.circulating_supply - sct_msg.value);

  if (sell_price < sct_msg.price_limit) {
    throw Error("Invalid sell price limit {} less than sell price {}",
                sct_msg.price_limit, sell_price);
  }

  account[sct_msg.contract_id] -= sct_msg.value;
  account[contract.base_contract_id] += sell_price;

  contract.circulating_supply -= sct_msg.value;
  contract.total_supply -= sct_msg.value;
}

void Handler::apply_spend_token(const AccountID& account_id,
                                const SpendTokenMsg& spend_msg,
                                const Hash& tx_hash, int64_t timestamp)
{
  auto& account = ctx.get<Account>(account_id);
  auto& contract = ctx.get<Contract>(spend_msg.token_key);
  auto& revenue = ctx.get<Revenue>(contract.revenue_id);

  const uint64_t tpc = revenue.get_tpc(timestamp);
  const uint256_t circulating_supply = contract.circulating_supply;
  const uint256_t new_circulating_supply = circulating_supply - spend_msg.value;

  const uint256_t sell_price = contract.get_sell_price(circulating_supply) -
                               contract.get_sell_price(new_circulating_supply);

  account[spend_msg.token_key] -= spend_msg.value;
  const uint256_t new_revenue = revenue.get_period_revenue(tpc) + sell_price;

  contract.circulating_supply = new_circulating_supply;
  revenue.set_period_revenue(tpc, new_revenue);
}

void Handler::apply_create_revenue(const AccountID& account_id,
                                   const CreateRevenueMsg& cr_msg,
                                   const Hash& tx_hash, int64_t timestamp)
{
  RevenueID revenue_id = RevenueID::from_addr(ed25519_vk_to_addr(tx_hash));
  ctx.create<Revenue>(revenue_id, cr_msg.base_token_id, account_id,
                      cr_msg.time_period, cr_msg.stake_id, cr_msg.is_private);
}
