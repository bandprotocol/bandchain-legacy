#pragma once

#include "band/msg.h"
#include "inc/essential.h"
#include "store/context.h"
#include "util/bytes.h"
#include "util/iban.h"

class Handler
{
public:
  friend class HandlerTest;

  Handler(Context& _ctx);

  void init(const std::string& init_state);

  /// Apply the given message to the blockchain.
  void apply_message(const MsgHdr& addr, Buffer& buf, const Hash& tx_hash,
                     int64_t timestamp);

private:
  /// Actual transaction processing.
  void apply_mint(const AccountID& addr, const MintMsg& mint_msg,
                  const Hash& tx_hash, int64_t timestamp);
  void apply_tx(const AccountID& addr, const TxMsg& tx_msg, const Hash& tx_hash,
                int64_t timestamp);
  void apply_create_contract(const AccountID& addr,
                             const CreateContractMsg& cc_msg,
                             const Hash& tx_hash, int64_t timestamp);
  void apply_purchase_contract(const AccountID& addr,
                               const PurchaseContractMsg& pct_msg,
                               const Hash& tx_hash, int64_t timestamp);
  void apply_sell_contract(const AccountID& addr,
                           const SellContractMsg& sct_msg, const Hash& tx_hash,
                           int64_t timestamp);
  void apply_spend_token(const AccountID& addr, const SpendTokenMsg& spend_msg,
                         const Hash& tx_hash, int64_t timestamp);
  void apply_create_revenue(const AccountID& addr,
                            const CreateRevenueMsg& cr_msg, const Hash& tx_hash,
                            int64_t timestamp);

private:
  Context& ctx;
};
