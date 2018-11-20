// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the LICENSE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#include "explorer.h"

#include <ctime>
#include <soci/soci.h>

using soci::use;

/// Struct to store row data in TokenTransaction table in database.
struct TokenTransaction {
  Ident srcUserID;
  Ident dstUserID;
  Ident tokenID;
  uint256_t value = 0;
  std::string txType;
  Hash txHash;
  uint64_t createdAt = 0;
  uint64_t blockNo = 0;
};

namespace
{
constexpr char queryMsg[] =
    "INSERT INTO transaction(src_user_id, dst_user_id, token_id, "
    "value, tx_type, tx_hash, created_at, block_no) VALUES(:src_user_id, "
    ":dst_user_id, :token_id, :value, :tx_type, :tx_hash, :created_at, "
    ":block_no)";
}

namespace soci
{
template <>
struct type_conversion<TokenTransaction> {
  typedef values base_type;

  static void from_base(const values& v, indicator ind, TokenTransaction& tx)
  {
    tx.srcUserID = v.get<std::string>("src_user_id", "");
    tx.dstUserID = v.get<std::string>("dst_user_id", "");
    tx.tokenID = v.get<std::string>("token_id");
    tx.value = uint256_t{v.get<std::string>("value")};
    tx.txType = v.get<std::string>("tx_type");
    tx.txHash = Hash::hex(v.get<std::string>("tx_hash"));
    auto tmp_tm = v.get<std::tm>("created_at");
    tx.createdAt = std::mktime(&tmp_tm);
    tx.blockNo = v.get<unsigned long>("block_no");
  }

  static void to_base(const TokenTransaction& tx, values& v, indicator& ind)
  {
    v.set("src_user_id", tx.srcUserID.to_string(),
          tx.srcUserID.empty() ? i_null : i_ok);
    v.set("dst_user_id", tx.dstUserID.to_string(),
          tx.dstUserID.empty() ? i_null : i_ok);
    v.set("token_id", tx.tokenID.to_string());
    v.set("value", tx.value.str());
    v.set("tx_type", tx.txType);
    v.set("tx_hash", tx.txHash.to_string());
    std::tm utc;
    time_t time = tx.createdAt;
    gmtime_r(&time, &utc);
    v.set("created_at", utc);
    v.set("block_no", tx.blockNo);
  }
};
} // namespace soci

ExplorerListener::ExplorerListener(soci::session& _database)
    : database(_database)
{
}

void ExplorerListener::begin(const BlockMsg& blk)
{
  tx.reset();
  tx = std::make_unique<soci::transaction>(database);
}

void ExplorerListener::commit(const BlockMsg& blk)
{
  tx->commit();
  tx.reset();
}

void ExplorerListener::handleMintToken(const BlockMsg& blk,
                                       const HeaderMsg& hdr,
                                       const MintTokenMsg& msg,
                                       const MintTokenResponse& res)
{
  TokenTransaction tx{Ident{}, hdr.user, msg.token,     msg.value,
                      "mint",  hdr.hash, blk.timestamp, blk.height};
  database << queryMsg, use(tx);
}

void ExplorerListener::handleTransferToken(const BlockMsg& blk,
                                           const HeaderMsg& hdr,
                                           const TransferTokenMsg& msg,
                                           const TransferTokenResponse& res)
{

  TokenTransaction tx{hdr.user,   msg.dest, msg.token,     msg.value,
                      "transfer", hdr.hash, blk.timestamp, blk.height};
  database << queryMsg, use(tx);
}

void ExplorerListener::handleBuyToken(const BlockMsg& blk,
                                      const HeaderMsg& hdr,
                                      const BuyTokenMsg& msg,
                                      const BuyTokenResponse& res)
{
  TokenTransaction txIn{hdr.user, Ident{},  res.baseToken, res.spent,
                        "buy",    hdr.hash, blk.timestamp, blk.height};
  TokenTransaction txOut{Ident{}, hdr.user, msg.token,     msg.value,
                         "buy",   hdr.hash, blk.timestamp, blk.height};
  database << queryMsg, use(txIn);
  database << queryMsg, use(txOut);
}

void ExplorerListener::handleSellToken(const BlockMsg& blk,
                                       const HeaderMsg& hdr,
                                       const SellTokenMsg& msg,
                                       const SellTokenResponse& res)
{
  TokenTransaction txIn{hdr.user, Ident{},  msg.token,     msg.value,
                        "sell",   hdr.hash, blk.timestamp, blk.height};
  TokenTransaction txOut{Ident{}, hdr.user, res.baseToken, res.received,
                         "sell",  hdr.hash, blk.timestamp, blk.height};

  database << queryMsg, use(txIn);
  database << queryMsg, use(txOut);
}
