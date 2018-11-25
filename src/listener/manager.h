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

#include <vector>

#include "inc/essential.h"
#include "listener/base.h"
#include "listener/primary.h"
#include "util/bytes.h"

/// ValidatorUpdate structure is used to notify Tendermint application about
/// validator set update. The primary listener has total control over how
/// this is implemented. See its implementation for more details.
struct ValidatorUpdate {
  /// The verify key of the validator to update the voting power
  VerifyKey verifyKey{};

  /// True if this verify key is added to the set. False otherwise.
  bool isAdd = false;
};

/// ListenerManager allows any listener to subscribe to blockchain information
/// via a common interface. The listener works both in live and historical
/// modes. Note that at any time, there can be one 'primary' listener that is
/// responsible for publishing data back to Tendermint consensus if run in
/// live mode.
class ListenerManager
{
public:
  /// Return stringified json representing the blockchain message interface.
  /// External clients are expected to call this to learn the messaging format.
  static std::string abi();

public:
  /// Load the states of all the listeners. This must be called after all
  /// listeners are added and before the consensus system starts.
  void loadStates();

  /// Initialize blockchain state according to the given genesis struct.
  void initChain(gsl::span<const byte> raw);

  /// Notify all the listeners that a new block has been proposed.
  void beginBlock(uint64_t timestamp, const Address& proposer);

  /// Notify the primary listener to check the given transaction message. Fail
  /// if no primary listener is set.
  void checkTransaction(gsl::span<const byte> raw);

  /// Notify the listeners to apply the given transaction message. Optionally
  /// take the result span if running in historical mode. Return the result of
  /// applying the transaction.
  std::string applyTransaction(gsl::span<const byte> raw,
                               gsl::span<const byte> rawResult = {});

  /// Notify the primary listener to end the current block and return the
  /// validator set update. Fail if called when the primary listener does not
  /// exist.
  std::vector<ValidatorUpdate> endBlock();

  /// Notify the listeners to commit all the applied messages persistently on
  /// chain.
  void commitBlock();

  /// Set the primary listener of this manager. Fail horribly if the primary
  /// listener already exists.
  void setPrimary(std::unique_ptr<PrimaryListener> primary);

  /// Add the given listener to this manager. Fail if the given listener is
  /// a primary listener.
  void addListener(std::unique_ptr<BaseListener> listener);

private:
  /// The primary listener of this ListenerManager. Only relevant if run in
  /// live mode.
  std::unique_ptr<PrimaryListener> primary;

  /// All other listeners of this manager. For active validators, it is
  /// advisable to not add any other listeners for maximum performance.
  std::vector<std::unique_ptr<BaseListener>> listeners;

  /// The information the most recent block. Use this timestamp for all
  /// applying transactions until the block ends.
  BlockMsg block;
};
