#include "handler.h"

#include "crypto/ed25519.h"
#include "store/account.h"
#include "store/ccontract.h"
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
    case CreateMsg::ID:
      apply_create(addr, buf.read_all<CreateMsg>(), tx_hash);
      break;
    case PurchaseCTMsg::ID:
      apply_purchaseCT(addr, buf.read_all<PurchaseCTMsg>(), tx_hash);
      break;
    case SellCTMsg::ID:
      apply_sellCT(addr, buf.read_all<SellCTMsg>(), tx_hash);
      break;
    default:
      throw Error("Invalid MsgID {}", uint16_t(hdr.msgid));
  }
}

void Handler::apply_mint(const Address& addr, const MintMsg& mint_msg,
                         const Hash& tx_hash)
{
  // Get the account view of this address.
  Account account(ctx, addr);

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

  // Get the account views of the source and destination addresses.
  Account account_source(ctx, addr);
  Account account_dest(ctx, tx_msg.dest);

  // Compute the new balances.
  const uint256_t new_source_balance =
      account_source.get_balance(tx_msg.token_key) - tx_msg.value;
  const uint256_t new_dest_balance =
      account_dest.get_balance(tx_msg.token_key) + tx_msg.value;

  // Update the information.
  account_source.set_balance(tx_msg.token_key, new_source_balance);
  account_dest.set_balance(tx_msg.token_key, new_dest_balance);
}

void Handler::apply_create(const Address& addr, const CreateMsg& create_msg,
                           const Hash& tx_hash)
{
  ContractID contractID = tx_hash.prefix<ContractID::Size>();
  CommunityContract contract(ctx, contractID);
  contract.set_equation(create_msg.curve);
}

void Handler::apply_purchaseCT(const Address& addr,
                               const PurchaseCTMsg& pct_msg,
                               const Hash& tx_hash)
{
  // Get the account and contract views
  Account account(ctx, addr);
  CommunityContract contract(ctx, pct_msg.contract_id);

  // Compute price of tokens
  const uint256_t current_supply = contract.get_current_supply();
  const uint256_t price =
      contract.apply_equation(current_supply + pct_msg.amount) -
      contract.apply_equation(current_supply);

  // Check price with band_limit
  if (price <= pct_msg.band_limit) {
    const uint256_t new_account_band_balance =
        account.get_band_balance() - price;
    const uint256_t new_account_ct_balance =
        account.get_balance(pct_msg.contract_id) + pct_msg.amount;
    const uint256_t new_current_supply = current_supply + pct_msg.amount;

    // Update the information
    account.set_band_balance(new_account_band_balance);
    account.set_balance(pct_msg.contract_id, new_account_ct_balance);
    contract.set_current_supply(new_current_supply);
  } else {
    throw Error("Price for purchase these community token ({}) is more than "
                "band_limit ({}), so you cannot purchase this amount of tokens",
                price, pct_msg.band_limit);
  }
}

void Handler::apply_sellCT(const Address& addr, const SellCTMsg& sellct_msg,
                           const Hash& tx_hash)
{
  // Get the account and contract views
  Account account(ctx, addr);
  CommunityContract contract(ctx, sellct_msg.contract_id);

  // Compute price of tokens
  const uint256_t current_supply = contract.get_current_supply();
  const uint256_t price =
      contract.apply_equation(current_supply) -
      contract.apply_equation(current_supply - sellct_msg.amount);

  // Check price with minimum band
  if (price >= sellct_msg.band_limit) {
    const uint256_t new_account_band_balance =
        account.get_band_balance() + price;
    const uint256_t new_account_ct_balance =
        account.get_balance(sellct_msg.contract_id) - sellct_msg.amount;
    const uint256_t new_current_supply = current_supply - sellct_msg.amount;

    // Update the information
    account.set_band_balance(new_account_band_balance);
    account.set_balance(sellct_msg.contract_id, new_account_ct_balance);
    contract.set_current_supply(new_current_supply);
  } else {
    throw Error("Now price({}) is lower than your minimum band that you want "
                "to sell ({}), so your sell request isn't processed.",
                price, sellct_msg.band_limit);
  }
}
