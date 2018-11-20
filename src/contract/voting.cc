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

// #include "voting.h"

// #include "contract/token.h"
// #include "crypto/sha256.h"
// #include "store/global.h"

// Voting::Voting(const Address& voting_id)
//     : Contract(voting_id, ContractID::Voting)
// {
// }

// void Voting::init(const Address& _token_id)
// {
//   token_id = _token_id;
// }

// void Voting::request_voting_power(uint256_t value)
// {
//   auto& token = Global::get().m_ctx->get<Token>(+token_id);
//   token.transfer(m_addr, value);
//   voting_power[get_sender()] = +voting_power[get_sender()] + value;
// }

// void Voting::withdraw_voting_power(uint256_t value)
// {
//   uint256_t available_token =
//       +voting_power[get_sender()] - get_maximum_vote(get_sender());
//   if (value > available_token)
//     throw Error("Not enough available token to withdraw, reveal or rescue
//     your "
//                 "vote first.");

//   auto& token = Global::get().m_ctx->get<Token>(+token_id);
//   voting_power[get_sender()] = +voting_power[get_sender()] - value;

//   // Remove from map
//   if (+voting_power[get_sender()] == 0) {
//     voting_power[get_sender()].erase();
//   }

//   Address receiver = get_sender();
//   set_sender();
//   token.transfer(receiver, value);
// }

// void Voting::rescue_token(uint256_t poll_id)
// {
//   assert_poll_exists(poll_id);

//   assert_vote_exist(get_sender(), poll_id);

//   assert_con(is_poll_ended(poll_id),
//              "You cannot rescue token in commit/reveal period.");

//   // Remove in votes and poll_list
//   auto& poll_receipt = m_poll[poll_id].votes[get_sender()];
//   // TODO: set
//   poll_list[get_sender()].erase({+poll_receipt.num_tokens, poll_id});
//   poll_receipt.erase();
// }

// void Voting::commit_vote(uint256_t poll_id, Hash secret_hash,
//                          uint256_t num_tokens)
// {
//   assert_con(is_commit_period(poll_id),
//              "Now you cannot commit vote to this poll.");
//   if (num_tokens > +voting_power[get_sender()]) {
//     throw Error("Your voting power in this contract less than your
//     request.");
//   }

//   // If already commit delete old vote
//   Address sender = get_sender();
//   if (did_commit(sender, poll_id)) {
//     uint256_t old_num_tokens = get_num_tokens(sender, poll_id);
//     m_poll[poll_id].votes[sender].erase();
//     poll_list[sender].erase({old_num_tokens, poll_id});
//   }
//   PollReceipt& poll_receipt = m_poll[poll_id].votes[sender];
//   poll_receipt.init(+num_tokens, secret_hash);

//   poll_list[sender].insert({num_tokens, poll_id});
// }

// void Voting::reveal_vote(uint256_t poll_id, bool vote_option, uint256_t salt)
// {
//   assert_con(is_reveal_period(poll_id),
//              "Now you cannot reveal vote in this poll.");
//   assert_con(did_commit(get_sender(), poll_id),
//              "You didn't committed vote in this poll.");
//   assert_con(!did_reveal(get_sender(), poll_id),
//              "You already revealed this vote.");

//   assert_con(get_hash(vote_option, salt) ==
//                  get_commit_hash(get_sender(), poll_id),
//              "Your vote doesn't match your commmit hash.");

//   uint256_t num_tokens = get_num_tokens(get_sender(), poll_id);
//   if (vote_option) {
//     m_poll[poll_id].votes_for = +m_poll[poll_id].votes_for + num_tokens;
//   } else {
//     m_poll[poll_id].votes_against = +m_poll[poll_id].votes_against +
//     num_tokens;
//   }

//   // After reveal change status in poll receipt and remove vote in address
//   poll
//   // list
//   m_poll[poll_id].votes[get_sender()].status = VotingStatus::Revealed;
//   poll_list[get_sender()].erase({+num_tokens, poll_id});
// }

// // Callable query function

// uint256_t Voting::get_vote_for(uint256_t poll_id) const
// {
//   assert_poll_exists(poll_id);
//   return +m_poll[poll_id].votes_for;
// }

// uint256_t Voting::get_vote_against(uint256_t poll_id) const
// {
//   assert_poll_exists(poll_id);
//   return +m_poll[poll_id].votes_against;
// }

// uint8_t Voting::get_period(uint256_t poll_id) const
// {
//   assert_poll_exists(poll_id);
//   if (is_commit_period(poll_id))
//     return PollStatus::Commit;
//   if (is_reveal_period(poll_id))
//     return PollStatus::Reveal;
//   return PollStatus::End;
// }

// uint8_t Voting::get_result(uint256_t poll_id) const
// {
//   assert_con(is_poll_ended(poll_id), "Poll hasn't ended yet.");
//   Poll& poll = m_poll[poll_id];
//   uint256_t votes_for = +poll.votes_for;
//   uint256_t votes_against = +poll.votes_against;

//   if (votes_for + votes_against == 0)
//     return VoteResult::Reject;
//   uint256_t percent = (100 * votes_for) / (votes_for + votes_against);
//   if (percent < +poll.losing_threshold)
//     return VoteResult::Reject;
//   if (percent < +poll.winning_threshold)
//     return VoteResult::NoVote;
//   return VoteResult::Approve;
// }

// uint64_t Voting::get_commit_end_time(uint256_t poll_id) const
// {
//   assert_poll_exists(poll_id);
//   return +m_poll[poll_id].commit_end_time;
// }

// uint64_t Voting::get_reveal_end_time(uint256_t poll_id) const
// {
//   assert_poll_exists(poll_id);
//   return +m_poll[poll_id].reveal_end_time;
// }

// uint256_t Voting::get_voting_power(Address address) const
// {
//   return +voting_power[address];
// }

// uint256_t Voting::get_number_pass_token(const Address& address,
//                                         const uint256_t& poll_id,
//                                         const uint256_t& salt) const
// {
//   assert_con(is_poll_ended(poll_id), "Poll hasn't ended yet.");
//   assert_con(did_reveal(address, poll_id),
//              "This address didn't revealed vote.");

//   assert_con(get_hash(get_result(poll_id), salt) ==
//                  get_commit_hash(address, poll_id),
//              "Your vote is invalid or losing vote.");

//   return get_num_tokens(address, poll_id);
// }

// uint256_t Voting::start_poll(uint8_t losing_threshold,
//                              uint8_t winning_threahold,
//                              uint64_t commit_duration, uint64_t
//                              reveal_duration)
// {
//   poll_nonce = +poll_nonce + 1;
//   uint64_t commit_end_time = Global::get().block_time + commit_duration;
//   uint64_t reveal_end_time = commit_end_time + reveal_duration;

//   Poll& poll = m_poll[+poll_nonce];
//   poll.init(+commit_end_time, +reveal_end_time, +losing_threshold,
//             +winning_threahold);
//   return +poll_nonce;
// }

// uint256_t Voting::get_total_winning_token(const uint256_t& poll_id) const
// {
//   assert_poll_exists(poll_id);

//   assert_con(is_poll_ended(poll_id), "Poll hasn't ended yet.");

//   if (get_result(poll_id))
//     return +m_poll[poll_id].votes_for;
//   else
//     return +m_poll[poll_id].votes_against;
// }

// bool Voting::is_poll_ended(const uint256_t& poll_id) const
// {
//   assert_poll_exists(poll_id);
//   return is_expired(+m_poll[poll_id].reveal_end_time);
// }

// bool Voting::is_commit_period(const uint256_t& poll_id) const
// {
//   assert_poll_exists(poll_id);
//   return !is_expired(+m_poll[poll_id].commit_end_time);
// }

// bool Voting::is_reveal_period(const uint256_t& poll_id) const
// {
//   assert_poll_exists(poll_id);
//   return !is_expired(+m_poll[poll_id].reveal_end_time) &&
//          !is_commit_period(poll_id);
// }

// bool Voting::did_commit(const Address& address, const uint256_t& poll_id)
// const
// {
//   assert_poll_exists(poll_id);
//   return m_poll[poll_id].votes[address].exist();
// }

// bool Voting::did_reveal(const Address& address, const uint256_t& poll_id)
// const
// {
//   assert_poll_exists(poll_id);
//   assert_vote_exist(address, poll_id);
//   return +m_poll[poll_id].votes[address].status == VotingStatus::Revealed;
// }

// void Voting::assert_poll_exists(const uint256_t& poll_id) const
// {
//   if (!m_poll[poll_id].exist())
//     throw Error("This poll doesn't exist.");
// }

// void Voting::assert_vote_exist(const Address& address,
//                                const uint256_t& poll_id) const
// {
//   assert_poll_exists(poll_id);
//   if (!m_poll[poll_id].votes[address].exist()) {
//     throw Error("Not found your vote in poll_id {}.", poll_id);
//   }
// }
// Hash Voting::get_commit_hash(const Address& address,
//                              const uint256_t& poll_id) const
// {
//   assert_poll_exists(poll_id);
//   assert_vote_exist(address, poll_id);

//   return +m_poll[poll_id].votes[address].commit_hash;
// }

// uint256_t Voting::get_num_tokens(const Address& address,
//                                  const uint256_t& poll_id) const
// {
//   assert_poll_exists(poll_id);
//   assert_vote_exist(address, poll_id);

//   return +m_poll[poll_id].votes[address].num_tokens;
// }

// uint256_t Voting::get_maximum_vote(const Address& address) const
// {
//   // Check set is empty
//   if (poll_list[address].size() == 0)
//     return 0;

//   return poll_list[address].get_max().first;
// }

// bool Voting::is_expired(uint64_t terminate_time) const
// {
//   return Global::get().block_time > terminate_time;
// }

// Hash Voting::get_hash(bool vote, const uint256_t& salt) const
// {
//   Buffer buf;
//   buf << vote << salt;
//   return sha256(gsl::make_span(buf.to_raw_string()));
// }

// void Voting::debug_create() const
// {
//   DEBUG(log, "Voting contract created at {}", m_addr);
// }

// void Voting::debug_save() const
// {
//   DEBUG(log, "Total Poll: {}", +poll_nonce);

//   // for (auto& [poll_id, poll] : m_poll) {
//   //   DEBUG(log, "  Result in Poll {}", poll_id);
//   //   DEBUG(log, "    commit_end_time: {}", poll.commit_end_time);
//   //   DEBUG(log, "    reveal_end_time: {}", poll.reveal_end_time);
//   //   DEBUG(log, "    losing_threshold: {}", poll.losing_threshold);
//   //   DEBUG(log, "    winning_threshold: {}", poll.winning_threshold);
//   //   DEBUG(log, "    votes for: {}", poll.votes_for);
//   //   DEBUG(log, "    votes against: {}", poll.votes_against);
//   //   DEBUG(log, "  List of voters");
//   //   for (auto& [addr, poll_receipt] : poll.votes) {
//   //     DEBUG(log, "    {}: {} voted {}", poll_receipt.status._to_string(),
//   //     addr,
//   //           poll_receipt.num_tokens);
//   //   }
//   //   DEBUG(log, "------------------------------------------------------");
//   // }

//   // DEBUG(log, "Total Voter: {}", voting_power.size());
//   // for (auto& [addr, vp] : voting_power) {
//   //   DEBUG(log, "  {} has {}", addr, vp);
//   // }
//   // DEBUG(log, "======================================================");
// }
