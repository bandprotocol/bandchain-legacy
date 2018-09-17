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
  void apply_message(const MsgHdr& addr, Buffer& buf, const Hash& tx_hash,
                     int64_t timestamp);

private:
  /// Actual transaction processing.
  void apply_mint(const Address& addr, const MintMsg& mint_msg,
                  const Hash& tx_hash, int64_t timestamp);
  void apply_tx(const Address& addr, const TxMsg& tx_msg, const Hash& tx_hash,
                int64_t timestamp);
  void apply_create_contract(const Address& addr,
                             const CreateContractMsg& cc_msg,
                             const Hash& tx_hash, int64_t timestamp);
  void apply_purchase_contract(const Address& addr,
                               const PurchaseContractMsg& pct_msg,
                               const Hash& tx_hash, int64_t timestamp);
  void apply_sell_contract(const Address& addr, const SellContractMsg& sct_msg,
                           const Hash& tx_hash, int64_t timestamp);

private:
  Context& ctx;
};
