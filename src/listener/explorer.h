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

#pragma once

#include "inc/essential.h"
#include "listener/base.h"
#include "soci/soci.h"
#include "util/msg.h"

/// Explorer listener to store transaction about token transfer to external
/// database. It makes web-server (block explorer) can read this data without
/// connect to tendermint and band application.
class ExplorerListener : public BaseListener
{
public:
  ExplorerListener(soci::session& _database);

public:
  /// Load the current state of this listener. May involve block height now.
  void load() final {}

  /// Begin a new block. Start transaction to write in database and ensure if
  /// program down, tx(s) in this block will revert and not appear in database.
  void begin(const BlockMsg& blk) final;

  /// Commit transaction to ensure all tx in block save in database.
  void commit(const BlockMsg& blk) final;

  /// Create 1 transaction from null sender sending tokens to receiver
  void handleMintToken(const BlockMsg& blk,
                       const HeaderMsg& hdr,
                       const MintTokenMsg& msg,
                       const MintTokenResponse& res) final;

  /// Create 1 transaction from sender sendings tokens to receiver.
  void handleTransferToken(const BlockMsg& blk,
                           const HeaderMsg& hdr,
                           const TransferTokenMsg& msg,
                           const TransferTokenResponse& res) final;

  /// Create 2 transactions. One is sender sending base_tokens to null receiver,
  /// other is null sender sending new_tokens to sender.
  void handleBuyToken(const BlockMsg& blk,
                      const HeaderMsg& hdr,
                      const BuyTokenMsg& msg,
                      const BuyTokenResponse& res) final;

  /// Create 2 transactions. One is sender sending new_tokens to null receiver,
  /// other is null sender sending base_token to sender.
  void handleSellToken(const BlockMsg& blk,
                       const HeaderMsg& hdr,
                       const SellTokenMsg& msg,
                       const SellTokenResponse& res) final;

private:
  std::unique_ptr<soci::transaction> tx;
  soci::session& database;
};
