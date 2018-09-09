#pragma once

#include "band/msg.h"
#include "inc/essential.h"
#include "store/context.h"
#include "util/bytes.h"

class Handler
{
public:
  friend class HandlerTest;

  Handler(Context& _ctx);

  /// Apply the given message to the blockchain.
  void apply_message(const MsgHdr& addr, Buffer& buf, const Hash& tx_hash);

private:
  /// Actual transaction processing.
  void apply_mint(const Address& addr, const MintMsg& mint_msg,
                  const Hash& tx_hash);
  void apply_tx(const Address& addr, const TxMsg& tx_msg, const Hash& tx_hash);
  void apply_create(const Address& addr, const CreateMsg& create_msg,
                    const Hash& tx_hash);
  void apply_purchaseCT(const Address& addr, const PurchaseCTMsg& pct_msg,
                        const Hash& tx_hash);

private:
  Context& ctx;
};
