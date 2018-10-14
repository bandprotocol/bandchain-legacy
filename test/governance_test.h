#include <array>
#include <cxxtest/TestSuite.h>

#include "contract/account.h"
#include "contract/governance.h"
#include "contract/tcr.h"
#include "contract/token.h"
#include "contract/voting.h"
#include "crypto/ed25519.h"
#include "inc/essential.h"
#include "store/context_map.h"
#include "store/global.h"
#include "util/buffer.h"

class GovernanceTest : public CxxTest::TestSuite
{
public:
  void testCreateGovernance()
  {
    std::unique_ptr<ContextMap> ctx = std::make_unique<ContextMap>();
    Global::get().m_ctx = ctx.get();

    Address mrA = create_account();

    Address band =
        Address::from_hex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    Curve linear = Curve(std::make_unique<EqVar>());
    ctx->create<Token>(band, band, linear);

    Address voting_id = Address::rand();
    ctx->create<Voting>(voting_id, band);
    ctx->flush();

    mint_band(mrA, 1000);

    Address gvn_id = Address::rand();
    ctx->create<Governance>(gvn_id, band, voting_id, 30, 70, 1000, 100, 100);
    ctx->flush();

    auto gvn = &ctx->get<Governance>(gvn_id);

    TS_ASSERT_EQUALS(
        "losing_threshold: 30, winning_threshold: 70, min_deposit: 1000, "
        "commit_duration: 100, reveal_duration: 100",
        gvn->params.to_string());
  }

  void testCreateProposal()
  {
    std::unique_ptr<ContextMap> ctx = std::make_unique<ContextMap>();
    Global::get().m_ctx = ctx.get();

    Address mrA = create_account();

    Address band =
        Address::from_hex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    Curve linear = Curve(std::make_unique<EqVar>());
    ctx->create<Token>(band, band, linear);

    Address voting_id = Address::rand();
    ctx->create<Voting>(voting_id, band);
    ctx->flush();

    mint_band(mrA, 10000);

    Address gvn_id = Address::rand();
    ctx->create<Governance>(gvn_id, band, voting_id, 30, 70, 1000, 100, 100);
    ctx->flush();

    Address registry_id = Address::rand();
    ctx->create<Registry>(registry_id, band, voting_id, gvn_id, 50, 50, 100,
                          100, 100, 100);
    ctx->flush();
    auto gvn = &ctx->get<Governance>(gvn_id);
    auto account = &ctx->get<Account>(mrA);
    account->set_sender();
    RegistryParameters r{60, 50, 150, 100, 100, 150};
    Buffer buf;
    buf << r;
    gvn->propose_new_parameter(registry_id, buf);
    ctx->flush();
    gvn = &ctx->get<Governance>(gvn_id);
    auto token = &ctx->get<Token>(band);
    TS_ASSERT_EQUALS(1, gvn->nonce_proposal);
    TS_ASSERT_EQUALS(1, gvn->m_proposals[1].poll_id);
    TS_ASSERT_EQUALS(registry_id, gvn->m_proposals[1].contract_id);
    TS_ASSERT_EQUALS(mrA, gvn->m_proposals[1].proposer);
    buf = gvn->m_proposals[1].data;
    auto str = buf.read_all<RegistryParameters>().to_string();
    TS_ASSERT_EQUALS(
        "vote_quorum: 60, dispensation_percentage: 50, min_deposit: 150, "
        "apply_duration: 100, commit_duration: 100, reveal_duration: 150",
        str);
    TS_ASSERT_EQUALS(false, gvn->m_proposals[1].is_resolved);
    TS_ASSERT_EQUALS(9000, token->balance(mrA));
  }

  void testResolveProposal()
  {
    std::unique_ptr<ContextMap> ctx = std::make_unique<ContextMap>();
    Global::get().m_ctx = ctx.get();
    Global::get().block_time = 0;

    Address mrA = create_account();

    Address band =
        Address::from_hex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    Curve linear = Curve(std::make_unique<EqVar>());
    ctx->create<Token>(band, band, linear);

    Address voting_id = Address::rand();
    ctx->create<Voting>(voting_id, band);
    ctx->flush();

    mint_band(mrA, 10000);

    Address gvn_id = Address::rand();
    ctx->create<Governance>(gvn_id, band, voting_id, 30, 70, 1000, 100, 100);
    ctx->flush();

    Address registry_id = Address::rand();
    ctx->create<Registry>(registry_id, band, voting_id, gvn_id, 50, 50, 100,
                          100, 100, 100);
    ctx->flush();
    // Case 1 vote < losing_threshold ( 30 )
    auto gvn = &ctx->get<Governance>(gvn_id);
    auto account = &ctx->get<Account>(mrA);
    account->set_sender();
    RegistryParameters r{60, 50, 150, 100, 100, 150};
    Buffer buf;
    buf << r;
    auto poll_id = gvn->propose_new_parameter(registry_id, buf);
    ctx->flush();
    gvn = &ctx->get<Governance>(gvn_id);

    auto vote = &ctx->get<Voting>(voting_id);
    vote->m_poll.at(poll_id).votes_for = 20;
    vote->m_poll.at(poll_id).votes_against = 100;
    ctx->flush();

    Global::get().block_time = 210;
    vote = &ctx->get<Voting>(voting_id);
    gvn = &ctx->get<Governance>(gvn_id);
    TS_ASSERT_EQUALS(VoteResult::Reject, vote->get_result(poll_id));
    gvn->resolve_proposal(poll_id);
    ctx->flush();

    auto token = &ctx->get<Token>(band);
    TS_ASSERT_EQUALS(9000, token->balance(mrA));

    // Case 2 NoVote
    Global::get().block_time = 250;
    gvn = &ctx->get<Governance>(gvn_id);
    account = &ctx->get<Account>(mrA);
    account->set_sender();
    poll_id = gvn->propose_new_parameter(registry_id, buf);
    ctx->flush();
    gvn = &ctx->get<Governance>(gvn_id);

    vote = &ctx->get<Voting>(voting_id);
    vote->m_poll.at(poll_id).votes_for = 50;
    vote->m_poll.at(poll_id).votes_against = 50;
    ctx->flush();

    Global::get().block_time = 460;
    vote = &ctx->get<Voting>(voting_id);
    TS_ASSERT_EQUALS(VoteResult::NoVote, vote->get_result(poll_id));
    token = &ctx->get<Token>(band);
    TS_ASSERT_EQUALS(8000, token->balance(mrA));
    gvn->resolve_proposal(poll_id);
    ctx->flush();

    token = &ctx->get<Token>(band);
    TS_ASSERT_EQUALS(9000, token->balance(mrA));
    auto tcr = &ctx->get<Registry>(registry_id);
    TS_ASSERT_EQUALS(
        "vote_quorum: 50, dispensation_percentage: 50, min_deposit: 100, "
        "apply_duration: 100, commit_duration: 100, reveal_duration: 100",
        tcr->params.to_string());

    // Case 3 success
    Global::get().block_time = 400;
    gvn = &ctx->get<Governance>(gvn_id);
    account = &ctx->get<Account>(mrA);
    account->set_sender();
    poll_id = gvn->propose_new_parameter(registry_id, buf);
    ctx->flush();
    gvn = &ctx->get<Governance>(gvn_id);

    vote = &ctx->get<Voting>(voting_id);
    vote->m_poll.at(poll_id).votes_for = 1;
    vote->m_poll.at(poll_id).votes_against = 0;
    ctx->flush();

    Global::get().block_time = 610;
    vote = &ctx->get<Voting>(voting_id);
    TS_ASSERT_EQUALS(VoteResult::Approve, vote->get_result(poll_id));
    token = &ctx->get<Token>(band);
    TS_ASSERT_EQUALS(8000, token->balance(mrA));
    gvn->resolve_proposal(poll_id);
    ctx->flush();

    token = &ctx->get<Token>(band);
    TS_ASSERT_EQUALS(9000, token->balance(mrA));
    tcr = &ctx->get<Registry>(registry_id);
    TS_ASSERT_EQUALS(
        "vote_quorum: 60, dispensation_percentage: 50, min_deposit: 150, "
        "apply_duration: 100, commit_duration: 100, reveal_duration: 150",
        tcr->params.to_string());
  }

  void testChallengeAfterUpdate()
  {
    std::unique_ptr<ContextMap> ctx = std::make_unique<ContextMap>();
    Global::get().m_ctx = ctx.get();
    Global::get().block_time = 0;

    Address mrA = create_account();

    Address band =
        Address::from_hex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    Curve linear = Curve(std::make_unique<EqVar>());
    ctx->create<Token>(band, band, linear);

    Address voting_id = Address::rand();
    ctx->create<Voting>(voting_id, band);
    ctx->flush();

    mint_band(mrA, 10000);

    Address gvn_id = Address::rand();
    ctx->create<Governance>(gvn_id, band, voting_id, 30, 70, 1000, 100, 100);
    ctx->flush();

    Address registry_id = Address::rand();
    ctx->create<Registry>(registry_id, band, voting_id, gvn_id, 50, 50, 100,
                          100, 100, 100);
    ctx->flush();

    auto account = &ctx->get<Account>(mrA);
    auto tcr = &ctx->get<Registry>(registry_id);
    account->set_sender();
    uint256_t list_id = tcr->apply("The first member", 120);
    ctx->flush();

    // Upgrade parameter of tcr
    Global::get().block_time = 120;
    tcr = &ctx->get<Registry>(registry_id);
    tcr->update_status(list_id);

    Global::get().block_time = 130;

    RegistryParameters r{50, 50, 150, 100, 100, 100};
    account = &ctx->get<Account>(mrA);
    auto gvn = &ctx->get<Governance>(gvn_id);
    account->set_sender();
    Buffer buf;
    buf << r;
    uint256_t g_id = gvn->propose_new_parameter(registry_id, buf);
    ctx->flush();

    auto vote = &ctx->get<Voting>(voting_id);
    vote->m_poll.at(g_id).votes_for = 1;
    vote->m_poll.at(g_id).votes_against = 0;
    ctx->flush();

    Global::get().block_time = 530;
    gvn = &ctx->get<Governance>(gvn_id);
    gvn->resolve_proposal(g_id);
    ctx->flush();

    tcr = &ctx->get<Registry>(registry_id);
    Address ch = create_account();
    mint_band(ch, 10000);
    auto ac = &ctx->get<Account>(ch);
    auto token = &ctx->get<Token>(band);
    ac->set_sender();
    TS_ASSERT_EQUALS(10000, token->balance(ch));
    TS_ASSERT_EQUALS(1, tcr->m_listings.size());
    tcr->challenge(list_id, "NO MORE DEPOSIT");
    ctx->flush();

    tcr = &ctx->get<Registry>(registry_id);
    token = &ctx->get<Token>(band);

    // Remove from list
    TS_ASSERT_EQUALS(10000, token->balance(ch));
    TS_ASSERT_EQUALS(10000, token->balance(mrA));
    TS_ASSERT_EQUALS(0, tcr->m_listings.size());
  }

private:
  Address create_account(VerifyKey vk = VerifyKey::rand())
  {
    Global::get().m_ctx->create<Account>(vk);
    Global::get().m_ctx->flush();
    return ed25519_vk_to_addr(vk);
  }

  void mint_band(Address address, uint256_t value)
  {
    auto& account = Global::get().m_ctx->get<Account>(address);
    account.set_sender();
    auto& token = Global::get().m_ctx->get<Token>(
        Address::from_hex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
    token.mint(value);
    Global::get().m_ctx->flush();
  }
};
