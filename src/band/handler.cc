#include "handler.h"

#include "crypto/ed25519.h"
#include "store/account.h"
#include "util/bytes.h"

Handler::Handler(Context& _ctx)
    : ctx(_ctx)
{
}

void Handler::apply_message(const MsgHdr& hdr, Buffer& buf)
{
  /// Convert the verify key into the sender's address.
  const Address addr = ed25519_vk_to_addr(hdr.vk);

  switch (hdr.msgid) {
    case MintMsg::ID:
      apply_mint(addr, buf.read_all<MintMsg>());
      break;
    case TxMsg::ID:
      apply_tx(addr, buf.read_all<TxMsg>());
      break;
    default:
      throw Error("Invalid MsgID {}", uint16_t(hdr.msgid));
  }
}

void Handler::apply_mint(const Address& addr, const MintMsg& mint_msg)
{
  /// Get the account view of this address.
  Account account(ctx, addr);

  /// Compute the new balance.
  const uint256_t new_balance = account.get_band_balance() + mint_msg.value;

  /// Update the balance.
  account.set_band_balance(new_balance);
}

void Handler::apply_tx(const Address& addr, const TxMsg& tx_msg)
{
  if (addr == tx_msg.dest)
    throw Error("Cannot send token from/to the same address");

  /// Get the account views of the source and destination addresses.
  Account account_source(ctx, addr);
  Account account_dest(ctx, tx_msg.dest);

  /// Compute the new balances.
  const uint256_t new_source_balance =
      account_source.get_band_balance() - tx_msg.value;
  const uint256_t new_dest_balance =
      account_dest.get_band_balance() + tx_msg.value;

  /// Update the information.
  account_source.set_band_balance(new_source_balance);
  account_dest.set_band_balance(new_dest_balance);
}
