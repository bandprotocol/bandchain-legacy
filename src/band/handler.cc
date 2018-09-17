#include "handler.h"

#include <boost/scope_exit.hpp>

#include "crypto/ed25519.h"
#include "store/account.h"
#include "store/contract.h"
#include "util/bytes.h"

Handler::Handler(Context& _ctx)
    : ctx(_ctx)
{
}

void Handler::apply_message(const MsgHdr& hdr, Buffer& buf, const Hash& tx_hash,
                            int64_t timestamp)
{
  BOOST_SCOPE_EXIT(&ctx) { ctx.reset(); }
  BOOST_SCOPE_EXIT_END

  // Convert the verify key into the sender's address.
  const Address addr = ed25519_vk_to_addr(hdr.vk);

  switch (hdr.msgid) {
    case MintMsg::ID:
      apply_mint(addr, buf.read_all<MintMsg>(), tx_hash, timestamp);
      break;
    case TxMsg::ID:
      apply_tx(addr, buf.read_all<TxMsg>(), tx_hash, timestamp);
      break;
    case CreateContractMsg::ID:
      apply_create_contract(addr, buf.read_all<CreateContractMsg>(), tx_hash,
                            timestamp);
      break;
    case PurchaseContractMsg::ID:
      apply_purchase_contract(addr, buf.read_all<PurchaseContractMsg>(),
                              tx_hash, timestamp);
      break;
    case SellContractMsg::ID:
      apply_sell_contract(addr, buf.read_all<SellContractMsg>(), tx_hash,
                          timestamp);
      break;
    default:
      throw Error("Invalid MsgID {}", uint16_t(hdr.msgid));
  }

  ctx.flush();
}

void Handler::apply_mint(const Address& addr, const MintMsg& mint_msg,
                         const Hash& tx_hash, int64_t timestamp)
{
  auto& account = ctx.get_or_create<Account>(addr);
  account[mint_msg.token_key] += mint_msg.value;
}

void Handler::apply_tx(const Address& addr, const TxMsg& tx_msg,
                       const Hash& tx_hash, int64_t timestamp)
{
  auto& account_src = ctx.get<Account>(addr);
  auto& account_dst = ctx.get_or_create<Account>(tx_msg.dest);
  account_src[tx_msg.token_key] -= tx_msg.value;
  account_dst[tx_msg.token_key] += tx_msg.value;
}

void Handler::apply_create_contract(const Address& addr,
                                    const CreateContractMsg& cc_msg,
                                    const Hash& tx_hash, int64_t timestamp)
{
  ContractID contractID = tx_hash.prefix<ContractID::Size>();
  ctx.create<Contract>(contractID, cc_msg.revenue_id, cc_msg.curve,
                       cc_msg.max_supply, cc_msg.is_transferable,
                       cc_msg.is_discountable, cc_msg.beneficiary);
}

void Handler::apply_purchase_contract(const Address& addr,
                                      const PurchaseContractMsg& pct_msg,
                                      const Hash& tx_hash, int64_t timestamp)
{
  auto& account = ctx.get<Account>(addr);
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

  // TODO: Read from revenue
  const ContractID base_contract_id{};

  account[base_contract_id] -= buy_price;
  account[pct_msg.contract_id] += pct_msg.value;
  beneficiary[base_contract_id] += (buy_price - sell_price);

  contract.circulating_supply += pct_msg.value;
  contract.total_supply += pct_msg.value;
}

void Handler::apply_sell_contract(const Address& addr,
                                  const SellContractMsg& sct_msg,
                                  const Hash& tx_hash, int64_t timestamp)
{
  auto& account = ctx.get<Account>(addr);
  auto& contract = ctx.get<Contract>(sct_msg.contract_id);

  const uint256_t sell_price =
      contract.get_sell_price(contract.circulating_supply) -
      contract.get_sell_price(contract.circulating_supply - sct_msg.value);

  // TODO: Read from revenue
  const ContractID base_contract_id{};

  if (sell_price < sct_msg.price_limit) {
    throw Error("Invalid sell price limit {} less than sell price {}",
                sct_msg.price_limit, sell_price);
  }

  account[sct_msg.contract_id] -= sct_msg.value;
  account[base_contract_id] += sell_price;

  contract.circulating_supply -= sct_msg.value;
  contract.total_supply -= sct_msg.value;
}
