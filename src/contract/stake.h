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

#include <set>
#include <unordered_map>
#include <vector>

#include "inc/essential.h"
#include "store/contract.h"
#include "store/data.h"
#include "store/mapping.h"
#include "store/set.h"
#include "store/vector.h"

class Stake : public Contract
{
public:
  friend class StakeTest;
  Stake(const Address& stake_id);

  void init(const Address& _base_token);

  // Callable functions
  // Stake token to dest (Validator) and create a new receipt.
  uint256_t stake(uint256_t party_id, uint256_t value);

  // Withdraw all token from receipt and delete that receipt.
  void withdraw(uint256_t receipt_id);

  // Create new party and stake some token to it.
  uint256_t create_party(uint256_t value, uint256_t numerator,
                         uint256_t denominator);

  // Sender get reward from token that they stake from stark stake or last
  // claim_reward until now.
  void claim_reward(uint256_t receipt_id);

  void reactivate_party(uint256_t party_id);

  // Query
  std::vector<Address> topx(uint16_t value);

  // Add reward from block that dest proposed block. (Store for claiming reward)
  void add_reward(uint256_t party_id, uint256_t value);

  // Deactivate party that didn't voted
  void deactivate_party(uint256_t party_id);

  void destroy_party(uint256_t party_id);

  void debug_create() const final;
  void debug_save() const final;

public:
  Data<Address> base_token{sha256(m_addr, uint16_t(1))};

private:
  struct Receipt {

    Receipt(const Hash& _parent_hash)
        : parent_hash(_parent_hash)
    {
    }

    void init(uint64_t _last_update_time, const Address& _owner,
              const uint256_t& _party_id, const uint256_t& _value)
    {
      _exist = true;
      last_update_time = _last_update_time;
      owner = _owner;
      party_id = _party_id;
      value = _value;
    }

    void erase()
    {
      _exist.erase();
      last_update_time.erase();
      owner.erase();
      party_id.erase();
      value.erase();
    }

    bool exist() { return _exist.exist(); }

    const Hash parent_hash;

    Data<bool> _exist{parent_hash};
    Data<uint64_t> last_update_time{sha256(parent_hash, uint(1))};
    Data<Address> owner{sha256(parent_hash, uint(2))};
    Data<uint256_t> party_id{sha256(parent_hash, uint(3))};
    Data<uint256_t> value{sha256(parent_hash, uint(4))};
  };

  struct Party {
    Party(const Hash _parent_hash)
        : parent_hash(_parent_hash)
    {
    }

    const Hash parent_hash;

    void init(const uint256_t& _last_checkpoint_stake,
              const uint256_t& _current_stake, const Address& _leader,
              const uint256_t& _numerator, const uint256_t& _denominator)
    {
      _exist = true;
      last_checkpoint_stake = _last_checkpoint_stake;
      current_stake = _current_stake;
      leader = _leader;
      numerator = _numerator;
      denominator = _denominator;
      is_active = true;
    }

    void erase()
    {
      _exist.erase();
      last_checkpoint_stake.erase();
      current_stake.erase();
      leader.erase();
      numerator.erase();
      denominator.erase();
      is_active.erase();
      sum_reward.destroy();
    }

    bool exist() { return _exist.exist(); }

    Data<bool> _exist{parent_hash};
    Data<uint256_t> last_checkpoint_stake{sha256(parent_hash, uint16_t(1))};
    Data<uint256_t> current_stake{sha256(parent_hash, uint16_t(2))};
    Data<Address> leader{sha256(parent_hash, uint16_t(3))};
    Data<uint256_t> numerator{sha256(parent_hash, uint16_t(4))};
    Data<uint256_t> denominator{sha256(parent_hash, uint16_t(5))};
    Data<bool> is_active{sha256(parent_hash, uint16_t(6))};
    Vector<std::pair<uint64_t, uint256_t>> sum_reward{
        sha256(parent_hash, uint16_t(7))};
  };

  Data<uint256_t> receipt_nonce{sha256(m_addr, uint16_t(2))};
  Data<uint256_t> party_nonce{sha256(m_addr, uint16_t(3))};

  const uint256_t magnitude{"100000000000000000000000000000000000"};

  Mapping<uint256_t, Receipt> m_receipts{sha256(m_addr, uint16_t(4))};
  Mapping<uint256_t, Party> m_parties{sha256(m_addr, uint16_t(5))};
  Set<std::pair<uint256_t, uint256_t>> active_party_list{
      sha256(m_addr, uint16_t(6))};

  static inline auto log = logger::get("stake");
};
