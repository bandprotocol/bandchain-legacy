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

#include <array>
#include <cxxtest/TestSuite.h>

#include "contract/account.h"
#include "contract/governance.h"
#include "contract/tcr.h"
#include "contract/token.h"
#include "contract/voting.h"
#include "crypto/ed25519.h"
#include "inc/essential.h"
#include "store/context.h"
#include "store/global.h"
#include "store/storage_map.h"
#include "util/buffer.h"

class GovernanceTest : public CxxTest::TestSuite
{
public:
  void testCreateGovernance()
  {
    std::unique_ptr<Storage> store = std::make_unique<StorageMap>();
    Context ctx(*store);
    Global::get().m_ctx = &ctx;
    Global::get().flush = true;

    Address mrA = create_account();

    Address band =
        Address::from_hex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    Curve linear = Curve(std::make_unique<EqVar>());
    {
      ctx.create<Token>(band, band, linear);
    }

    Address voting_id = Address::rand();
    {
      ctx.create<Voting>(voting_id, band);
    }

    mint_band(mrA, 1000);

    Address gvn_id = Address::rand();
    ctx.create<Governance>(gvn_id, band, voting_id, 30, 70, 1000, 100, 100);

    auto gvn = &ctx.get<Governance>(gvn_id);

    TS_ASSERT_EQUALS(
        "losing_threshold: 30, winning_threshold: 70, min_deposit: 1000, "
        "commit_duration: 100, reveal_duration: 100",
        gvn->params.to_string());
  }

  void testCreateProposal()
  {
    std::unique_ptr<Storage> store = std::make_unique<StorageMap>();
    Context ctx(*store);
    Global::get().m_ctx = &ctx;

    Address mrA = create_account();

    Address band =
        Address::from_hex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    Curve linear = Curve(std::make_unique<EqVar>());
    {
      ctx.create<Token>(band, band, linear);
    }

    Address voting_id = Address::rand();
    {
      ctx.create<Voting>(voting_id, band);
    }

    mint_band(mrA, 10000);

    Address gvn_id = Address::rand();
    {
      ctx.create<Governance>(gvn_id, band, voting_id, 30, 70, 1000, 100, 100);
    }

    Address registry_id = Address::rand();
    {
      ctx.create<Registry>(registry_id, band, voting_id, gvn_id, 50, 50, 100,
                           100, 100, 100);
    }

    {
      auto gvn = &ctx.get<Governance>(gvn_id);
      auto account = &ctx.get<Account>(mrA);
      account->set_sender();
      Buffer buf;
      add_buffer(buf, uint16_t(60), uint16_t(50), uint16_t(150), uint16_t(100),
                 uint16_t(100), uint16_t(150));
      gvn->propose_new_parameter(registry_id, buf);
    }
    {
      auto gvn = &ctx.get<Governance>(gvn_id);
      auto token = &ctx.get<Token>(band);
      TS_ASSERT_EQUALS(1, +gvn->nonce_proposal);
      TS_ASSERT_EQUALS(1, +gvn->m_proposals[1].poll_id);
      TS_ASSERT_EQUALS(registry_id, +gvn->m_proposals[1].contract_id);
      TS_ASSERT_EQUALS(mrA, +gvn->m_proposals[1].proposer);
      Buffer buf = +gvn->m_proposals[1].data;
      RegistryParameters r{Hash()};
      auto str = r.parse_buffer(buf);
      TS_ASSERT_EQUALS(
          "vote_quorum: 60, dispensation_percentage: 50, min_deposit: 150, "
          "apply_duration: 100, commit_duration: 100, reveal_duration: 150",
          str);
      TS_ASSERT_EQUALS(false, +gvn->m_proposals[1].is_resolved);
      TS_ASSERT_EQUALS(9000, token->balance(mrA));
    }
  }

  void testResolveProposal()
  {
    std::unique_ptr<Storage> store = std::make_unique<StorageMap>();
    Context ctx(*store);
    Global::get().m_ctx = &ctx;
    Global::get().flush = true;

    Address mrA = create_account();

    Address band =
        Address::from_hex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    Curve linear = Curve(std::make_unique<EqVar>());
    {
      ctx.create<Token>(band, band, linear);
    }
    Address voting_id = Address::rand();
    {
      ctx.create<Voting>(voting_id, band);
    }

    mint_band(mrA, 10000);

    Address gvn_id = Address::rand();
    {
      ctx.create<Governance>(gvn_id, band, voting_id, 30, 70, 1000, 100, 100);
    }

    Address registry_id = Address::rand();
    {
      ctx.create<Registry>(registry_id, band, voting_id, gvn_id, 50, 50, 100,
                           100, 100, 100);
    }
    // Case 1 vote < losing_threshold ( 30 )
    uint256_t poll_id;
    Buffer buf;
    {
      auto gvn = &ctx.get<Governance>(gvn_id);
      auto account = &ctx.get<Account>(mrA);
      account->set_sender();

      add_buffer(buf, uint16_t(60), uint16_t(50), uint16_t(150), uint16_t(100),
                 uint16_t(100), uint16_t(150));
      poll_id = gvn->propose_new_parameter(registry_id, buf);
    }

    {
      auto vote = &ctx.get<Voting>(voting_id);
      vote->m_poll[poll_id].votes_for = 20;
      vote->m_poll[poll_id].votes_against = 100;
    }

    Global::get().block_time = 210;
    {
      auto vote = &ctx.get<Voting>(voting_id);
      auto gvn = &ctx.get<Governance>(gvn_id);
      TS_ASSERT_EQUALS(VoteResult::Reject, vote->get_result(poll_id));
      gvn->resolve_proposal(poll_id);
    }

    {
      auto token = &ctx.get<Token>(band);
      TS_ASSERT_EQUALS(9000, token->balance(mrA));
      auto tcr = &ctx.get<Registry>(registry_id);
      TS_ASSERT_EQUALS(
          "vote_quorum: 50, dispensation_percentage: 50, min_deposit: 100, "
          "apply_duration: 100, commit_duration: 100, reveal_duration: 100",
          tcr->params.to_string());
    }

    // Case 2 NoVote
    Global::get().block_time = 250;
    {
      auto gvn = &ctx.get<Governance>(gvn_id);
      auto account = &ctx.get<Account>(mrA);
      account->set_sender();
      poll_id = gvn->propose_new_parameter(registry_id, buf);
    }

    {
      auto vote = &ctx.get<Voting>(voting_id);
      vote->m_poll[poll_id].votes_for = 50;
      vote->m_poll[poll_id].votes_against = 50;
    }

    Global::get().block_time = 460;
    {
      auto gvn = &ctx.get<Governance>(gvn_id);
      auto vote = &ctx.get<Voting>(voting_id);
      TS_ASSERT_EQUALS(VoteResult::NoVote, vote->get_result(poll_id));
      auto token = &ctx.get<Token>(band);
      TS_ASSERT_EQUALS(8000, token->balance(mrA));
      gvn->resolve_proposal(poll_id);
    }

    {
      auto token = &ctx.get<Token>(band);
      TS_ASSERT_EQUALS(9000, token->balance(mrA));
      auto tcr = &ctx.get<Registry>(registry_id);
      TS_ASSERT_EQUALS(
          "vote_quorum: 50, dispensation_percentage: 50, min_deposit: 100, "
          "apply_duration: 100, commit_duration: 100, reveal_duration: 100",
          tcr->params.to_string());
    }

    // Case 3 success
    Global::get().block_time = 400;
    {
      auto gvn = &ctx.get<Governance>(gvn_id);
      auto account = &ctx.get<Account>(mrA);
      account->set_sender();
      poll_id = gvn->propose_new_parameter(registry_id, buf);
    }
    {
      auto vote = &ctx.get<Voting>(voting_id);
      vote->m_poll[poll_id].votes_for = 1;
      vote->m_poll[poll_id].votes_against = 0;
    }

    Global::get().block_time = 610;
    {
      auto gvn = &ctx.get<Governance>(gvn_id);
      auto vote = &ctx.get<Voting>(voting_id);
      TS_ASSERT_EQUALS(VoteResult::Approve, vote->get_result(poll_id));
      auto token = &ctx.get<Token>(band);
      TS_ASSERT_EQUALS(8000, token->balance(mrA));
      gvn->resolve_proposal(poll_id);
    }

    {
      auto token = &ctx.get<Token>(band);
      TS_ASSERT_EQUALS(9000, token->balance(mrA));
      auto tcr = &ctx.get<Registry>(registry_id);
      TS_ASSERT_EQUALS(
          "vote_quorum: 60, dispensation_percentage: 50, min_deposit: 150, "
          "apply_duration: 100, commit_duration: 100, reveal_duration: 150",
          tcr->params.to_string());
    }
  }

  void testChallengeAfterUpdate()
  {
    std::unique_ptr<Storage> store = std::make_unique<StorageMap>();
    Address mrA;
    Address band;
    {
      Context ctx(*store);
      Global::get().m_ctx = &ctx;
      Global::get().flush = true;
      Global::get().block_time = 0;

      mrA = create_account();

      band = Address::from_hex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
      Curve linear = Curve(std::make_unique<EqVar>());
      ctx.create<Token>(band, band, linear);

      Address voting_id = Address::rand();
      ctx.create<Voting>(voting_id, band);

      mint_band(mrA, 10000);

      Address gvn_id = Address::rand();
      ctx.create<Governance>(gvn_id, band, voting_id, 30, 70, 1000, 100, 100);

      Address registry_id = Address::rand();
      ctx.create<Registry>(registry_id, band, voting_id, gvn_id, 50, 50, 100,
                           100, 100, 100);

      auto account = &ctx.get<Account>(mrA);
      auto tcr = &ctx.get<Registry>(registry_id);
      account->set_sender();
      uint256_t list_id = tcr->apply("The first member", 120);

      // Upgrade parameter of tcr
      Global::get().block_time = 120;
      tcr = &ctx.get<Registry>(registry_id);
      tcr->update_status(list_id);

      Global::get().block_time = 130;

      account = &ctx.get<Account>(mrA);
      auto gvn = &ctx.get<Governance>(gvn_id);
      account->set_sender();
      Buffer buf;
      add_buffer(buf, uint16_t(50), uint16_t(50), uint16_t(150), uint16_t(100),
                 uint16_t(100), uint16_t(100));
      uint256_t g_id = gvn->propose_new_parameter(registry_id, buf);

      auto vote = &ctx.get<Voting>(voting_id);
      vote->m_poll[g_id].votes_for = 1;
      vote->m_poll[g_id].votes_against = 0;

      Global::get().block_time = 530;
      gvn = &ctx.get<Governance>(gvn_id);
      gvn->resolve_proposal(g_id);

      tcr = &ctx.get<Registry>(registry_id);
      Address ch = create_account();
      mint_band(ch, 10000);
      auto ac = &ctx.get<Account>(ch);
      auto token = &ctx.get<Token>(band);
      ac->set_sender();
      TS_ASSERT_EQUALS(9880, token->balance(mrA));
      tcr->challenge(list_id, "NO MORE DEPOSIT");

      tcr = &ctx.get<Registry>(registry_id);
      token = &ctx.get<Token>(band);

      // Remove from list
      TS_ASSERT_EQUALS(10000, token->balance(ch));
      TS_ASSERT_EQUALS(10000, token->balance(mrA));
    }

    // Create new context that use same storage
    {
      Context ctx(*store);
      Global::get().m_ctx = &ctx;
      auto token = &ctx.get<Token>(band);
      TS_ASSERT_EQUALS(10000, token->balance(mrA));
    }
  }

private:
  Address create_account(VerifyKey vk = VerifyKey::rand())
  {
    Global::get().m_ctx->create<Account>(ed25519_vk_to_addr(vk), vk);
    return ed25519_vk_to_addr(vk);
  }

  void mint_band(Address address, uint256_t value)
  {
    auto& account = Global::get().m_ctx->get<Account>(address);
    account.set_sender();
    auto& token = Global::get().m_ctx->get<Token>(
        Address::from_hex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
    token.mint(value);
  }
};
