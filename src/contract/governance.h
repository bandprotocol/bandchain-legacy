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

#include <unordered_map>

#include "inc/essential.h"
#include "store/contract.h"
#include "store/data.h"
#include "store/mapping.h"
#include "store/upgradable.h"

struct GovernanceParameters {

  GovernanceParameters(const Hash& _parent_hash)
      : parent_hash(_parent_hash)
  {
  }

  void init(uint8_t _losing_threshold,
            uint8_t _winning_threshold,
            uint256_t _min_deposit,
            uint64_t _commit_duration,
            uint64_t _reveal_duration)
  {
    losing_threshold = _losing_threshold;
    winning_threshold = _winning_threshold;
    min_deposit = _min_deposit;
    commit_duration = _commit_duration;
    reveal_duration = _reveal_duration;
  }

  void upgrade(Buffer buf)
  {
    losing_threshold = buf.read<uint8_t>();
    winning_threshold = buf.read<uint8_t>();
    min_deposit = buf.read<uint256_t>();
    commit_duration = buf.read<uint64_t>();
    reveal_duration = buf.read<uint64_t>();
  }
  std::string parse_buffer(Buffer buf) const
  {
    auto _losing_threshold = buf.read<uint8_t>();
    auto _winning_threshold = buf.read<uint8_t>();
    auto _min_deposit = buf.read<uint256_t>();
    auto _commit_duration = buf.read<uint64_t>();
    auto _reveal_duration = buf.read<uint64_t>();
    return "losing_threshold: {}, winning_threshold: {}, min_deposit: {}, commit_duration: {}, reveal_duration: {}"_format(
        _losing_threshold, _winning_threshold, _min_deposit, _commit_duration,
        _reveal_duration);
  }
  std::string to_string() const
  {
    return "losing_threshold: {}, winning_threshold: {}, min_deposit: {}, commit_duration: {}, reveal_duration: {}"_format(
        +losing_threshold, +winning_threshold, +min_deposit, +commit_duration,
        +reveal_duration);
  }
  bool exist()
  {
    return _exist.exist();
  }

  const Hash parent_hash;

  Data<bool> _exist{parent_hash};
  Data<uint8_t> losing_threshold{sha256(parent_hash, uint16_t(1))};
  Data<uint8_t> winning_threshold{sha256(parent_hash, uint16_t(2))};
  Data<uint256_t> min_deposit{sha256(parent_hash, uint16_t(3))};
  Data<uint64_t> commit_duration{sha256(parent_hash, uint16_t(4))};
  Data<uint64_t> reveal_duration{sha256(parent_hash, uint16_t(5))};
};

// inline Buffer& operator>>(Buffer& buf, GovernanceParameters& params)
// {
//   return buf >> params.losing_threshold >> params.winning_threshold >>
//          params.min_deposit >> params.commit_duration >>
//          params.reveal_duration;
// }
// inline Buffer& operator<<(Buffer& buf, const GovernanceParameters& params)
// {
//   return buf << params.losing_threshold << params.winning_threshold
//              << params.min_deposit << params.commit_duration
//              << params.reveal_duration;
// }

class Governance : public UpgradableContract<GovernanceParameters>
{
public:
  friend class GovernanceTest;
  Governance(const Address& governance_id);

  void init(const Address& _token_id,
            const Address& _voting_id,
            uint8_t _losing_threshold,
            uint8_t _winning_threshold,
            const uint256_t& _min_deposit,
            uint64_t _commit_duration,
            uint64_t _reveal_duration);
  // Callable function
  uint256_t propose_new_parameter(Address contract_id, Buffer buffer);

  // uint256_t propose_governance_parameter(Buffer buffer);

  void resolve_proposal(uint256_t proposal_id);

  void debug_create() const final;

  void debug_save() const final;

public:
  Data<Address> token_id{sha256(m_addr, uint16_t(1))};
  Data<Address> voting_id{sha256(m_addr, uint16_t(2))};

private:
  Data<uint256_t> nonce_proposal{sha256(m_addr, uint16_t(3))};

  struct Proposal {
    Proposal(const Hash& _parent_hash)
        : parent_hash(_parent_hash)
    {
    }

    void init(const uint256_t& _poll_id,
              const Address& _contract_id,
              const Address& _proposer,
              Buffer _data,
              const std::string& _detail)
    {
      _exist = true;
      poll_id = _poll_id;
      contract_id = _contract_id;
      proposer = _proposer;
      data = _data;
      detail = _detail;
      is_resolved = false;
    }

    bool exist()
    {
      return _exist.exist();
    }

    Hash parent_hash;

    Data<bool> _exist{parent_hash};
    Data<uint256_t> poll_id{sha256(parent_hash, uint16_t(1))};
    Data<Address> contract_id{sha256(parent_hash, uint16_t(2))};
    Data<Address> proposer{sha256(parent_hash, uint16_t(3))};
    Data<Buffer> data{sha256(parent_hash, uint16_t(4))};
    Data<std::string> detail{sha256(parent_hash, uint16_t(5))};
    Data<bool> is_resolved{sha256(parent_hash, uint16_t(6))};
  };

  Mapping<uint256_t, Proposal> m_proposals{sha256(m_addr, uint16_t(4))};
  Mapping<Address, Data<bool>> in_progress{sha256(m_addr, uint16_t(5))};

  static inline auto log = logger::get("governance");
};
