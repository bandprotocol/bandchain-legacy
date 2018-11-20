// // Licensed to the Apache Software Foundation (ASF) under one
// // or more contributor license agreements.  See the LICENSE file
// // distributed with this work for additional information
// // regarding copyright ownership.  The ASF licenses this file
// // to you under the Apache License, Version 2.0 (the
// // "License"); you may not use this file except in compliance
// // with the License.  You may obtain a copy of the License at
// //
// //   http://www.apache.org/licenses/LICENSE-2.0
// //
// // Unless required by applicable law or agreed to in writing,
// // software distributed under the License is distributed on an
// // "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// // KIND, either express or implied.  See the License for the
// // specific language governing permissions and limitations
// // under the License.

// #pragma once

// #include <unordered_map>

// #include "inc/essential.h"
// #include "store/contract.h"
// #include "store/data.h"
// #include "store/mapping.h"
// #include "store/upgradable.h"

// struct RegistryParameters {

//   RegistryParameters(const Hash& _parent_hash)
//       : parent_hash(_parent_hash)
//   {
//   }

//   void init(uint8_t _vote_quorum,
//             uint8_t _dispensation_percentage,
//             uint256_t _min_deposit,
//             uint64_t _apply_duration,
//             uint64_t _commit_duration,
//             uint64_t _reveal_duration)
//   {
//     vote_quorum = _vote_quorum;
//     dispensation_percentage = _dispensation_percentage;
//     min_deposit = _min_deposit;
//     apply_duration = _apply_duration;
//     commit_duration = _commit_duration;
//     reveal_duration = _reveal_duration;
//   }

//   void upgrade(Buffer buf)
//   {
//     vote_quorum = buf.read<uint8_t>();
//     dispensation_percentage = buf.read<uint8_t>();
//     min_deposit = buf.read<uint256_t>();
//     apply_duration = buf.read<uint64_t>();
//     commit_duration = buf.read<uint64_t>();
//     reveal_duration = buf.read<uint64_t>();
//   }

//   std::string to_string() const
//   {
//     return "vote_quorum: {}, dispensation_percentage: {}, min_deposit: {},
//     apply_duration: {}, commit_duration: {}, reveal_duration: {}"_format(
//         +vote_quorum, +dispensation_percentage, +min_deposit,
//         +apply_duration, +commit_duration, +reveal_duration);
//   }

//   std::string parse_buffer(Buffer buf) const
//   {
//     auto _vote_quorum = buf.read<uint8_t>();
//     auto _dispensation_percentage = buf.read<uint8_t>();
//     auto _min_deposit = buf.read<uint256_t>();
//     auto _apply_duration = buf.read<uint64_t>();
//     auto _commit_duration = buf.read<uint64_t>();
//     auto _reveal_duration = buf.read<uint64_t>();

//     return "vote_quorum: {}, dispensation_percentage: {}, min_deposit: {},
//     apply_duration: {}, commit_duration: {}, reveal_duration: {}"_format(
//         _vote_quorum, _dispensation_percentage, _min_deposit,
//         _apply_duration, _commit_duration, _reveal_duration);
//   }

//   bool exist()
//   {
//     return _exist.exist();
//   }

//   const Hash parent_hash;

//   Data<bool> _exist{parent_hash};
//   Data<uint8_t> vote_quorum{sha256(parent_hash, uint16_t(1))};
//   Data<uint8_t> dispensation_percentage{sha256(parent_hash, uint16_t(2))};
//   Data<uint256_t> min_deposit{sha256(parent_hash, uint16_t(3))};
//   Data<uint64_t> apply_duration{sha256(parent_hash, uint16_t(4))};
//   Data<uint64_t> commit_duration{sha256(parent_hash, uint16_t(5))};
//   Data<uint64_t> reveal_duration{sha256(parent_hash, uint16_t(6))};
// };

// // inline Buffer& operator>>(Buffer& buf, RegistryParameters& params)
// // {
// //   return buf >> params.vote_quorum >> params.dispensation_percentage >>
// //          params.min_deposit >> params.apply_duration >>
// //          params.commit_duration >> params.reveal_duration;

// //   params.vote_quorum = buf.read<uint8_t>();
// //   params.dispensation_percentage = buf.read<uint8_t>();
// //   params.min_deposit = buf.read<uint256_t>();
// //   params.apply_duration = buf.read<uint64_t>();
// //   params.commit_duration = buf.read<uint64_t>();
// //   params.reveal_duration = buf.read<uint64_t>();
// //   return buf;
// // }
// // inline Buffer& operator<<(Buffer& buf, const RegistryParameters& params)
// // {
// //   return buf << params.vote_quorum << params.dispensation_percentage
// //              << params.min_deposit << params.apply_duration
// //              << params.commit_duration << params.reveal_duration;
// // }

// class Registry : public UpgradableContract<RegistryParameters>
// {
// public:
//   friend class GovernanceTest;

//   Registry(const Address& registry_id);

//   void init(const Address& _token_id,
//             const Address& _voting_id,
//             const Address& _governance_id,
//             uint8_t _vote_quorum,
//             uint8_t _dispensation_percentage,
//             const uint256_t& _min_deposit,
//             uint64_t _apply_duration,
//             uint64_t _commit_duration,
//             uint64_t _reveal_duration);

//   // Callable function
//   uint256_t apply(std::string data, uint256_t token_deposit);

//   void deposit(uint256_t list_id, uint256_t token_deposit);

//   void withdraw(uint256_t list_id, uint256_t token_withdraw);

//   void exit(uint256_t list_id);

//   uint256_t challenge(uint256_t list_id, std::string data);

//   void update_status(uint256_t list_id);

//   void claim_reward(uint256_t challenge_id, uint256_t salt);

//   // Function for get information
//   uint256_t voter_reward(const Address& voter,
//                          const uint256_t challenge_id,
//                          const uint256_t& salt) const;

//   bool can_in_list(const uint256_t& list_id) const;

//   bool is_listed(const uint256_t& list_id) const;

//   bool has_been_challenge(const uint256_t& list_id) const;

//   bool challenge_can_be_resolved(const uint256_t& list_id) const;

//   uint256_t determine_reward(const uint256_t& challenge_id) const;

//   bool token_claimed(const uint256_t& challenge_id, const Address& voter)
//   const;

//   void debug_create() const final;

//   void debug_save() const final;

//   // Callable query function

//   Address get_voting_id() const;

//   uint256_t active_list_length() const;

//   uint256_t active_list_id_at(uint256_t index) const;

//   std::string get_content(uint256_t list_id) const;

//   uint256_t get_deposit(uint256_t list_id) const;

//   uint256_t get_active_challenge(uint256_t list_id) const;

//   bool need_update(uint256_t list_id) const;

//   uint256_t get_poll_id(uint256_t challenge_id) const;

//   bool is_proposal(uint256_t list_id) const;

//   uint64_t get_app_expire(uint256_t list_id) const;

//   Address get_list_owner(uint256_t list_id) const;

//   Address get_challenger_id(uint256_t challenge_id) const;

//   std::string get_reason(uint256_t challenge_id) const;

// private:
//   void resolve_challenge(const uint256_t& list_id);

//   void set_to_list(const uint256_t& list_id);

//   void reset_listing(const uint256_t& list_id);

//   bool list_exist(const uint256_t& list_id) const;

//   bool challenge_exist(const uint256_t& challenge_id) const;

// public:
//   Data<Address> token_id{sha256(m_addr, uint16_t(1))};
//   Data<Address> voting_id{sha256(m_addr, uint16_t(2))};

// private:
//   Data<uint256_t> listing_nonce{sha256(m_addr, uint16_t(3))};
//   Data<uint256_t> challenge_nonce{sha256(m_addr, uint16_t(4))};

//   struct Listing {
//     Listing(const Hash& _parent_hash)
//         : parent_hash(_parent_hash)
//     {
//     }

//     void init(uint64_t _app_expire_time,
//               const Address& _owner,
//               const uint256_t& _unstake_deposit,
//               const std::string& _data)
//     {
//       _exist = true;
//       app_expire_time = _app_expire_time;
//       is_listed = false;
//       owner = _owner;
//       unstake_deposit = _unstake_deposit;
//       challenge_id = 0;
//       data = _data;
//     }

//     void erase()
//     {
//       _exist.erase();
//       app_expire_time.erase();
//       is_listed.erase();
//       owner.erase();
//       unstake_deposit.erase();
//       challenge_id.erase();
//       data.erase();
//     }

//     bool exist()
//     {
//       return _exist.exist();
//     }

//     const Hash parent_hash;

//     Data<bool> _exist{parent_hash};
//     Data<uint64_t> app_expire_time{sha256(parent_hash, uint16_t(1))};
//     Data<bool> is_listed{sha256(parent_hash, uint16_t(2))};
//     Data<Address> owner{sha256(parent_hash, uint16_t(3))};
//     Data<uint256_t> unstake_deposit{sha256(parent_hash, uint16_t(4))};
//     Data<uint256_t> challenge_id{sha256(parent_hash, uint16_t(5))};
//     Data<std::string> data{sha256(parent_hash, uint16_t(6))};
//   };

//   struct Challenge {
//     Challenge(const Hash& _parent_hash)
//         : parent_hash(_parent_hash)
//     {
//     }

//     void init(const uint256_t& _poll_id,
//               const Address& _challenger_address,
//               const std::string& _data,
//               const uint256_t& reward,
//               const uint256_t& _stake)
//     {
//       _exist = true;
//       poll_id = _poll_id;
//       challenger_address = _challenger_address;
//       data = _data;
//       remaining_reward_pool = reward;
//       is_resolved = false;
//       stake = _stake;
//       remaining_winning_token = 0;
//     }
//     bool exist()
//     {
//       return _exist.exist();
//     }

//     const Hash parent_hash;

//     Data<bool> _exist{parent_hash};
//     Data<uint256_t> poll_id{sha256(parent_hash, uint16_t(1))};
//     Data<Address> challenger_address{sha256(parent_hash, uint16_t(2))};
//     Data<std::string> data{sha256(parent_hash, uint16_t(3))};

//     Data<uint256_t> remaining_reward_pool{sha256(parent_hash, uint16_t(4))};
//     Data<bool> is_resolved{sha256(parent_hash, uint16_t(5))};
//     Data<uint256_t> stake{sha256(parent_hash, uint16_t(6))};
//     Data<uint256_t> remaining_winning_token{sha256(parent_hash,
//     uint16_t(7))}; Mapping<Address, Data<bool>> token_claimed{
//         sha256(parent_hash, uint16_t(8))};
//   };

//   Mapping<uint256_t, Challenge> m_challenges{sha256(m_addr, uint16_t(5))};
//   Mapping<uint256_t, Listing> m_listings{sha256(m_addr, uint16_t(6))};

//   static inline auto log = logger::get("registry");
// };
