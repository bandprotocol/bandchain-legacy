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

// #include "governance.h"

// #include "contract/token.h"
// #include "contract/voting.h"
// #include "store/global.h"

// Governance::Governance(const Address& governance_id)
//     : UpgradableContract(governance_id, ContractID::Governance)

// {
// }

// void Governance::init(const Address& _token_id, const Address& _voting_id,
//                       uint8_t _losing_threshold, uint8_t _winning_threshold,
//                       const uint256_t& _min_deposit, uint64_t
//                       _commit_duration, uint64_t _reveal_duration)
// {
//   auto& vote = Global::get().m_ctx->get<Voting>(_voting_id);
//   if (+vote.token_id != _token_id) {
//     throw Error("Token type doesn't match with voting contract");
//   }

//   token_id = _token_id;
//   voting_id = _voting_id;
//   nonce_proposal = 0;

//   params.init(_losing_threshold, _winning_threshold, _min_deposit,
//               _commit_duration, _reveal_duration);
//   governance_id = m_addr;
// }

// uint256_t Governance::propose_new_parameter(Address contract_id, Buffer
// buffer)
// {
//   assert_con(!in_progress[contract_id].exist(),
//              "This registry 's parameter is in progress.");

//   auto& upgradable_object =
//   Global::get().m_ctx->get<Upgradable>(contract_id);

//   assert_con(upgradable_object.get_governance_id() == m_addr,
//              "Governance id of TCR contract doesn't match this address.");
//   // Transfer min_deposit to contract
//   Token& token = Global::get().m_ctx->get<Token>(+token_id);
//   token.transfer(m_addr, +params.min_deposit);

//   // Create poll
//   Voting& vote = Global::get().m_ctx->get<Voting>(+voting_id);
//   uint256_t poll_id =
//       vote.start_poll(+params.losing_threshold, +params.winning_threshold,
//                       +params.commit_duration, +params.reveal_duration);

//   // We need to read buffer to params first?
//   Buffer buf = buffer;
//   // Create Proposal
//   nonce_proposal = +nonce_proposal + 1;
//   if (m_proposals[+nonce_proposal].exist()) {
//     throw Failure("This nonce already have data.");
//   }
//   Proposal& proposal = m_proposals[+nonce_proposal];
//   proposal.init(poll_id, contract_id, get_sender(), buffer,
//                 upgradable_object.to_string(buf));

//   in_progress[contract_id] = true;
//   return +nonce_proposal;
// }

// void Governance::resolve_proposal(uint256_t proposal_id)
// {
//   assert_con(m_proposals[proposal_id].exist(), "Proposal not found");

//   Proposal& proposal = m_proposals[proposal_id];
//   assert_con(!(+proposal.is_resolved), "Proposal was resolved already");

//   auto& vote = Global::get().m_ctx->get<Voting>(+voting_id);
//   uint8_t result = vote.get_result(+proposal.poll_id);
//   switch (result) {
//     case VoteResult::Approve: {
//       set_sender();
//       Token& token = Global::get().m_ctx->get<Token>(+token_id);
//       token.transfer(+proposal.proposer, +params.min_deposit);

//       Global::get()
//           .m_ctx->get<Upgradable>(+proposal.contract_id)
//           .upgrade(+proposal.data);
//       break;
//     }
//     case VoteResult::NoVote: {
//       // Don't update parameter and send money back
//       set_sender();
//       Token& token = Global::get().m_ctx->get<Token>(+token_id);
//       token.transfer(+proposal.proposer, +params.min_deposit);
//       break;
//     }
//     case VoteResult::Reject:
//       // Don't update and don't send money back

//       break;
//     default:
//       throw Failure("No matching vote option.");
//   }

//   proposal.is_resolved = true;
//   in_progress[+proposal.contract_id].erase();
// }

// void Governance::debug_create() const
// {
//   DEBUG(log, "Governance contract created at {}", m_addr);
// }

// void Governance::debug_save() const
// {
//   // DEBUG(log, "Total Proposals: {}", m_proposals.size());

//   // for (auto& [proposal_id, proposal] : m_proposals) {
//   //   DEBUG(log, "  ProposalID {}", proposal_id);
//   //   DEBUG(log, "    poll_id: {}", proposal.poll_id);
//   //   DEBUG(log, "    contract_id: {}", proposal.contract_id);
//   //   DEBUG(log, "    proposer: {}", proposal.proposer);
//   //   DEBUG(log, "    is_resolved: {}", proposal.is_resolved);
//   //   DEBUG(log, "    parameters {}:", proposal.detail);
//   // }

//   // DEBUG(log, "------------------------------------------------------");

//   // DEBUG(log, "Total In progress: {}", in_progress.size());
//   // for (auto& addr : in_progress) {
//   //   DEBUG(log, "  Address: ", addr.first);
//   // }
//   // DEBUG(log, "======================================================");
// }
