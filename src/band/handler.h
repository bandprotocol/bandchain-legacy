#pragma once

#include "band/msg.h"
#include "store/context.h"

class Handler
{
public:
  Handler(Context& _ctx);

  /// Apply the given message to the blockchain.
  void apply_message(const MsgHdr& addr, Buffer& buf);

private:
  /// Actual transaction processing.
  void apply_mint(const Address& addr, const MintMsg& mint_msg);
  void apply_tx(const Address& addr, const TxMsg& tx_msg);

private:
  Context& ctx;
};
