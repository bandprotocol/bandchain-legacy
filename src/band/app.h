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
#include "net/tmapp.h"
#include "store/context.h"

class BandApplication : public TendermintApplication
{
public:
  BandApplication(Context& _ctx);

  std::string get_name() const final
  {
    return "band";
  }
  std::string get_version() const final
  {
    return "latest";
  }

  /// Return the current application merkle tree hash.
  std::string get_current_app_hash() const final;

  /// Initialize the blockchain according to the genesis information.
  void init(const std::vector<std::pair<VerifyKey, uint64_t>>& _validators,
            const std::string& init_state) final;

  /// Query the blockchain information. Data must be a JSON-serialized string
  /// following BAND RPC specification.
  std::string query(const std::string& path, const std::string& data) final;

  /// Check the transaction. For now it does nothing.
  void check(const std::string& msg_raw) final;

  /// Apply the transaction to the blockchain.
  std::string apply(const std::string& msg_raw) final;

  void begin_block(uint64_t block_time, const Address& block_proposer) final;

  std::vector<std::pair<VerifyKey, uint64_t>> end_block() final;

  void commit_block() final;

private:
  Context& ctx;
  bool use_fake_time;
  // Handler handler;
  // Query qry;
};
