#include <cxxtest/TestSuite.h>

#include "contract/account.h"
#include "contract/token.h"
#include "contract/voting.h"
#include "crypto/ed25519.h"
#include "inc/essential.h"
#include "store/context_map.h"
#include "store/global.h"
#include "util/buffer.h"

class VotingTest : public CxxTest::TestSuite
{
public:
  void testCreateVote()
  {
    std::unique_ptr<ContextMap> ctx = std::make_unique<ContextMap>();
    Global::get().m_ctx = ctx.get();

    Address band =
        Address::from_hex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    Curve linear = Curve(std::make_unique<EqVar>());

    ctx->create<Token>(band, band, linear);
    Address voting = Address::rand();
    ctx->create<Voting>(voting, band);
    ctx->flush();

    auto vote = &ctx->get<Voting>(voting);
    auto poll_id = vote->start_poll(50, 50, 10, 10);
    TS_ASSERT_EQUALS(1, poll_id);
    ctx->flush();
  }

  void testRequestVote()
  {
    std::unique_ptr<ContextMap> ctx = std::make_unique<ContextMap>();
    Global::get().m_ctx = ctx.get();

    Address band =
        Address::from_hex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    Curve linear = Curve(std::make_unique<EqVar>());
    ctx->create<Token>(band, band, linear);

    VerifyKey x = VerifyKey::rand();
    Address mrA = ed25519_vk_to_addr(x);
    ctx->create<Account>(x);

    Address voting = Address::rand();
    ctx->create<Voting>(voting, band);
    ctx->flush();

    auto account = &ctx->get<Account>(mrA);
    auto token = &ctx->get<Token>(band);
    auto vote = &ctx->get<Voting>(voting);
    account->set_sender();
    token->mint(1000);
    ctx->flush();

    vote = &ctx->get<Voting>(voting);
    account = &ctx->get<Account>(mrA);
    token = &ctx->get<Token>(band);
    account->set_sender();
    vote->request_voting_power(100);
    ctx->flush();

    vote = &ctx->get<Voting>(voting);
    account = &ctx->get<Account>(mrA);
    token = &ctx->get<Token>(band);
    account->set_sender();
    TS_ASSERT_EQUALS(900, token->balance(mrA));
    TS_ASSERT_EQUALS(100, token->balance(voting));
    vote->withdraw_voting_power(50);
    ctx->flush();

    vote = &ctx->get<Voting>(voting);
    account = &ctx->get<Account>(mrA);
    token = &ctx->get<Token>(band);
    account->set_sender();
    TS_ASSERT_EQUALS(950, token->balance(mrA));
  }

  void testCommitVote()
  {
    std::unique_ptr<ContextMap> ctx = std::make_unique<ContextMap>();
    Global::get().m_ctx = ctx.get();

    Address band =
        Address::from_hex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    Curve linear = Curve(std::make_unique<EqVar>());
    ctx->create<Token>(band, band, linear);

    VerifyKey x = VerifyKey::rand();
    Address mrA = ed25519_vk_to_addr(x);
    ctx->create<Account>(x);

    x = VerifyKey::rand();
    Address mrB = ed25519_vk_to_addr(x);
    ctx->create<Account>(x);

    Address voting = Address::rand();
    ctx->create<Voting>(voting, band);
    ctx->flush();

    auto account = &ctx->get<Account>(mrA);
    auto accountb = &ctx->get<Account>(mrB);
    auto token = &ctx->get<Token>(band);
    auto vote = &ctx->get<Voting>(voting);
    account->set_sender();
    token->mint(1000);
    ctx->flush();

    vote = &ctx->get<Voting>(voting);
    account = &ctx->get<Account>(mrA);
    token = &ctx->get<Token>(band);
    account->set_sender();
    vote->request_voting_power(100);
    ctx->flush();

    vote = &ctx->get<Voting>(voting);
    account = &ctx->get<Account>(mrA);
    token = &ctx->get<Token>(band);
    auto poll_id = vote->start_poll(50, 50, 10, 10);
    ctx->flush();

    vote = &ctx->get<Voting>(voting);
    account = &ctx->get<Account>(mrA);
    token = &ctx->get<Token>(band);
    account->set_sender();
    vote->commit_vote(poll_id, get_hash(true, 12), 100);
    ctx->flush();

    // vote = &ctx->get<Voting>(voting);
    // account = &ctx->get<Account>(mrA);
    // token = &ctx->get<Token>(band);
    // account->set_sender();
    // TS_ASSERT_THROWS_ANYTHING(vote->reveal_vote(1, true, 12));
    // ctx->reset();

    // Want to vote more
    vote = &ctx->get<Voting>(voting);
    account = &ctx->get<Account>(mrA);
    token = &ctx->get<Token>(band);
    account->set_sender();
    vote->request_voting_power(50);
    ctx->flush();

    vote = &ctx->get<Voting>(voting);
    account = &ctx->get<Account>(mrA);
    token = &ctx->get<Token>(band);
    account->set_sender();
    vote->commit_vote(poll_id, get_hash(true, 72), 149);
    ctx->flush();

    // Another account join vote
    vote = &ctx->get<Voting>(voting);
    accountb = &ctx->get<Account>(mrB);
    token = &ctx->get<Token>(band);
    accountb->set_sender();
    token->mint(1000);
    ctx->flush();

    vote = &ctx->get<Voting>(voting);
    accountb = &ctx->get<Account>(mrB);
    token = &ctx->get<Token>(band);
    accountb->set_sender();
    vote->request_voting_power(365);
    ctx->flush();

    vote = &ctx->get<Voting>(voting);
    accountb = &ctx->get<Account>(mrB);
    token = &ctx->get<Token>(band);
    accountb->set_sender();
    vote->commit_vote(poll_id, get_hash(false, 999), 300);
    ctx->flush();

    vote = &ctx->get<Voting>(voting);
    accountb = &ctx->get<Account>(mrB);
    token = &ctx->get<Token>(band);
    TS_ASSERT_EQUALS(515, token->balance(voting));
    TS_ASSERT(vote->did_commit(mrB, 1));
    TS_ASSERT(!vote->did_commit(band, 1));
    TS_ASSERT_EQUALS(515, token->balance(voting));
  }

  void testRevealVote()
  {
    std::unique_ptr<ContextMap> ctx = std::make_unique<ContextMap>();
    Global::get().m_ctx = ctx.get();
    Global::get().block_time = 0;
    Address band =
        Address::from_hex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    Curve linear = Curve(std::make_unique<EqVar>());
    ctx->create<Token>(band, band, linear);

    VerifyKey x = VerifyKey::rand();
    Address mrA = ed25519_vk_to_addr(x);
    ctx->create<Account>(x);

    x = VerifyKey::rand();
    Address mrB = ed25519_vk_to_addr(x);
    ctx->create<Account>(x);

    Address voting = Address::rand();
    ctx->create<Voting>(voting, band);
    ctx->flush();

    auto account = &ctx->get<Account>(mrA);
    auto accountb = &ctx->get<Account>(mrB);
    auto token = &ctx->get<Token>(band);
    auto vote = &ctx->get<Voting>(voting);
    account->set_sender();
    token->mint(1000);
    accountb->set_sender();
    token->mint(1000);
    ctx->flush();

    vote = &ctx->get<Voting>(voting);
    account = &ctx->get<Account>(mrA);
    token = &ctx->get<Token>(band);
    account->set_sender();
    vote->request_voting_power(150);
    accountb->set_sender();
    vote->request_voting_power(365);
    ctx->flush();

    vote = &ctx->get<Voting>(voting);
    account = &ctx->get<Account>(mrA);
    token = &ctx->get<Token>(band);
    auto poll_id = vote->start_poll(50, 50, 10, 10);
    ctx->flush();

    vote = &ctx->get<Voting>(voting);
    account = &ctx->get<Account>(mrA);
    token = &ctx->get<Token>(band);
    account->set_sender();
    vote->commit_vote(poll_id, get_hash(true, 72), 149);
    ctx->flush();

    vote = &ctx->get<Voting>(voting);
    accountb = &ctx->get<Account>(mrB);
    token = &ctx->get<Token>(band);
    accountb->set_sender();
    vote->commit_vote(1, get_hash(false, 999), 300);
    ctx->flush();

    vote = &ctx->get<Voting>(voting);
    account = &ctx->get<Account>(mrA);
    token = &ctx->get<Token>(band);
    account->set_sender();
    TS_ASSERT_THROWS_ANYTHING(vote->reveal_vote(poll_id, true, 12));
    ctx->reset();

    Global::get().block_time = 15;
    vote = &ctx->get<Voting>(voting);
    account = &ctx->get<Account>(mrA);
    token = &ctx->get<Token>(band);
    account->set_sender();
    vote->reveal_vote(poll_id, true, 72);
    ctx->flush();

    Global::get().block_time = 17;
    vote = &ctx->get<Voting>(voting);
    accountb = &ctx->get<Account>(mrB);
    token = &ctx->get<Token>(band);
    accountb->set_sender();
    vote->reveal_vote(poll_id, false, 999);
    ctx->flush();

    vote = &ctx->get<Voting>(voting);
    accountb = &ctx->get<Account>(mrB);
    token = &ctx->get<Token>(band);
    accountb->set_sender();
    vote->withdraw_voting_power(320);
    ctx->flush();

    vote = &ctx->get<Voting>(voting);
    accountb = &ctx->get<Account>(mrB);
    token = &ctx->get<Token>(band);
    TS_ASSERT_EQUALS(955, token->balance(mrB));
    TS_ASSERT_THROWS_ANYTHING(vote->get_result(poll_id));
    TS_ASSERT(!vote->is_poll_ended(poll_id));
  }

  Hash get_hash(bool vote, const uint256_t& salt) const
  {
    Buffer buf;
    buf << vote << salt;
    return sha256(gsl::make_span(buf.to_raw_string()));
  }
};
