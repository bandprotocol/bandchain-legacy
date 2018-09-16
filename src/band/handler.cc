#include "handler.h"

#include "crypto/ed25519.h"
#include "store/account.h"
#include "store/contract.h"
#include "util/bytes.h"

Handler::Handler(Context& _ctx)
    : ctx(_ctx)
{
}

void Handler::apply_message(const MsgHdr& hdr, Buffer& buf, const Hash& tx_hash)
{
  // Convert the verify key into the sender's address.
  const Address addr = ed25519_vk_to_addr(hdr.vk);

  switch (hdr.msgid) {
    case MintMsg::ID:
      apply_mint(addr, buf.read_all<MintMsg>(), tx_hash);
      break;
    case TxMsg::ID:
      apply_tx(addr, buf.read_all<TxMsg>(), tx_hash);
      break;
    case CreateContractMsg::ID:
      apply_create_contract(addr, buf.read_all<CreateContractMsg>(), tx_hash);
      break;
    case PurchaseContractMsg::ID:
      apply_purchase_contract(addr, buf.read_all<PurchaseContractMsg>(),
                              tx_hash);
      break;
    case SellContractMsg::ID:
      apply_sell_contract(addr, buf.read_all<SellContractMsg>(), tx_hash);
      break;
    default:
      throw Error("Invalid MsgID {}", uint16_t(hdr.msgid));
  }
}

void Handler::apply_mint(const Address& addr, const MintMsg& mint_msg,
                         const Hash& tx_hash)
{
  auto& account = ctx.get_or_create<Account>(addr);

  // Compute the new balance.
  const uint256_t new_balance =
      account.get_balance(mint_msg.token_key) + mint_msg.value;

  // Update the balance.
  account.set_balance(mint_msg.token_key, new_balance);
}

void Handler::apply_tx(const Address& addr, const TxMsg& tx_msg,
                       const Hash& tx_hash)
{
  if (addr == tx_msg.dest)
    throw Error("Cannot send token from/to the same address");

  auto& account_source = ctx.get_as<Account>(addr);
  auto& account_dest = ctx.get_or_create<Account>(tx_msg.dest);

  // Compute the new balances.
  const uint256_t new_source_balance =
      account_source.get_balance(tx_msg.token_key) - tx_msg.value;
  const uint256_t new_dest_balance =
      account_dest.get_balance(tx_msg.token_key) + tx_msg.value;

  // Update the information.
  account_source.set_balance(tx_msg.token_key, new_source_balance);
  account_dest.set_balance(tx_msg.token_key, new_dest_balance);
}

void Handler::apply_create_contract(const Address& addr,
                                    const CreateContractMsg& cc_msg,
                                    const Hash& tx_hash)
{
  ContractID contractID = tx_hash.prefix<ContractID::Size>();
  ctx.create<Contract>(contractID, cc_msg.revenue_id, cc_msg.curve,
                       cc_msg.max_supply, cc_msg.is_transferable,
                       cc_msg.is_discountable, cc_msg.beneficiary);
}

void Handler::apply_purchase_contract(const Address& addr,
                                      const PurchaseContractMsg& pct_msg,
                                      const Hash& tx_hash)
{
  auto& account = ctx.get_as<Account>(addr);
  auto& contract = ctx.get_as<Contract>(pct_msg.contract_id);

  const uint256_t remain_tokens =
      contract.get_max_supply() - contract.get_total_supply();

  if (remain_tokens < pct_msg.value) {
    throw Error("There aren't tokens enough to sell");
  }
  // Compute price of tokens
  const uint256_t circulating_supply = contract.get_circulating_supply();

  const uint256_t new_circulating_supply = circulating_supply + pct_msg.value;

  const uint256_t buy_price = contract.get_buy_price(new_circulating_supply) -
                              contract.get_buy_price(circulating_supply);
  const uint256_t sell_price = contract.get_sell_price(new_circulating_supply) -
                               contract.get_sell_price(circulating_supply);

  // Check price with band_limit
  if (buy_price > pct_msg.price_limit) {
    throw Error("Price for purchasing these community token ({}) is more than "
                "band_limit ({}), so you cannot purchase this number of tokens",
                buy_price, pct_msg.price_limit);
  }

  // TODO: Create revenue class and get base contract_id
  ContractID base_contract_id =
      ContractID::from_hex("0000000000000000000000000000000000000000");

  const uint256_t new_account_base_balance =
      account.get_balance(base_contract_id) - buy_price;
  const uint256_t new_account_out_balance =
      account.get_balance(pct_msg.contract_id) + pct_msg.value;

  const uint256_t new_total_supply =
      contract.get_total_supply() + pct_msg.value;

  const uint256_t spread_price = buy_price - sell_price;

  auto& beneficiary = ctx.get_as<Account>(contract.get_beneficiary());

  // Update the information
  account.set_balance(base_contract_id, new_account_base_balance);
  account.set_balance(pct_msg.contract_id, new_account_out_balance);

  const uint256_t new_beneficiary_balance =
      beneficiary.get_balance(base_contract_id) + spread_price;

  beneficiary.set_balance(base_contract_id, new_beneficiary_balance);

  contract.set_circulating_supply(new_circulating_supply);
  contract.set_total_supply(new_total_supply);
}

void Handler::apply_sell_contract(const Address& addr,
                                  const SellContractMsg& sct_msg,
                                  const Hash& tx_hash)
{
  auto& account = ctx.get_as<Account>(addr);
  auto& contract = ctx.get_as<Contract>(sct_msg.contract_id);

  // Compute price of tokens
  const uint256_t circulating_supply = contract.get_circulating_supply();
  const uint256_t new_circulating_supply = circulating_supply - sct_msg.value;
  const uint256_t new_total_supply =
      contract.get_total_supply() - sct_msg.value;

  const uint256_t sell_price = contract.get_sell_price(circulating_supply) -
                               contract.get_sell_price(new_circulating_supply);

  // TODO: Read from revenue
  const ContractID contract_id{};
  // Check price with minimum band
  if (sell_price < sct_msg.price_limit) {
    throw Error("Now price({}) is lower than your minimum band that you want "
                "to sell({}), so your sell request isn't processed.",
                sell_price, sct_msg.price_limit);
  }
  const uint256_t new_account_base_balance =
      account.get_balance(contract_id) + sell_price;
  const uint256_t new_account_out_balance =
      account.get_balance(sct_msg.contract_id) - sct_msg.value;

  // Update the information
  account.set_balance(contract_id, new_account_base_balance);
  account.set_balance(sct_msg.contract_id, new_account_out_balance);

  contract.set_circulating_supply(new_circulating_supply);
  contract.set_total_supply(new_total_supply);
}
