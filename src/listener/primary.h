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

#include <enum/enum.h>

#include "listener/base.h"
#include "util/msg.h"

/// The mode in which this primary listener is running. Switching back and
/// forth while the validator is running.
ENUM(PrimaryMode, uint8_t, None, Check, Apply)

/// Primary listener to maintain the blockchain state necessary for running
/// a fullnode without providing the query interface. This listener is
/// responsible for validating transactions and updating the validator set.
class PrimaryListener : public BaseListener
{
public:
  /// Switch the mode between checking and applying transactions.
  void switchMode(PrimaryMode mode);

  /// Validate the given set of transaction information. Mutate the user's
  /// nonce appropriately. Mode switching must be done before this is called.
  /// Throw exception if the validation fails.
  void validateTransaction(const HeaderMsg& hdr, gsl::span<const byte> data);

public:
  /// Create an account to RocksDB data storage. Throw if the creator does not
  /// have the necessary permission or the username is used.
  void handleCreateAccount(const HeaderMsg& hdr,
                           const CreateAccountMsg& msg) final;

  /// Create a token contract to RocksDB data storage. Throw if the token
  /// identifier is used.
  void handleCreateToken(const HeaderMsg& hdr, const CreateTokenMsg& msg) final;

  /// Add the appropriate amount of tokens to the transaction sender for the
  /// specified token contract.
  void handleMintToken(const HeaderMsg& hdr, const MintTokenMsg& msg) final;

  /// Transfer tokens from the sender to the given dest.
  void handleTransferToken(const HeaderMsg& hdr,
                           const TransferTokenMsg& msg) final;

  /// Convert the base tokens to the new tokens for the given amount.
  void handleBuyToken(const HeaderMsg& hdr, const BuyTokenMsg& msg) final;

  /// Convert the tokens back to the base tokens for the given amount.
  void handleSellToken(const HeaderMsg& hdr, const SellTokenMsg& msg) final;
};
