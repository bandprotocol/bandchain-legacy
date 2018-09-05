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
  // TODO
  TokenKey tokenkey = tx_hash.prefix<TokenKey::Size>();
  CommunityContract contract(ctx, tokenkey);
  contract.set_equation(create_msg.curve);
}
