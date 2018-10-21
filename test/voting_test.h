#include <cxxtest/TestSuite.h>

#include "contract/account.h"
#include "contract/token.h"
#include "contract/voting.h"
#include "crypto/ed25519.h"
#include "inc/essential.h"
#include "store/context.h"
#include "store/global.h"
#include "store/storage_map.h"
#include "util/buffer.h"

class VotingTest : public CxxTest::TestSuite
{
public:
  void testCreateVote()
  {
    std::unique_ptr<Storage> store = std::make_unique<StorageMap>();
    Context ctx(*store);
    Global::get().m_ctx = &ctx;

    Address band =
        Address::from_hex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    Curve linear = Curve(std::make_unique<EqVar>());

    ctx.create<Token>(band, band, linear);
    Address voting = Address::rand();
    ctx.create<Voting>(voting, band);
    ctx.flush();

    auto votep = &ctx.get<Voting>(voting);
    auto poll_id = votep->start_poll(50, 50, 10, 10);
    TS_ASSERT_EQUALS(1, poll_id);
  }

  void testRequestVote()
  {
    std::unique_ptr<Storage> store = std::make_unique<StorageMap>();
    Context ctx(*store);
    Global::get().m_ctx = &ctx;

    Address band =
        Address::from_hex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    Curve linear = Curve(std::make_unique<EqVar>());
    ctx.create<Token>(band, band, linear);

    VerifyKey x = VerifyKey::rand();
    Address mrA = ed25519_vk_to_addr(x);
    ctx.create<Account>(mrA, x);

    Address voting = Address::rand();
    ctx.create<Voting>(voting, band);
    ctx.flush();

    auto account = &ctx.get<Account>(mrA);
    auto token = &ctx.get<Token>(band);
    auto vote = &ctx.get<Voting>(voting);
    account->set_sender();
    token->mint(1000);
    ctx.flush();

    vote = &ctx.get<Voting>(voting);
    account = &ctx.get<Account>(mrA);
    token = &ctx.get<Token>(band);
    account->set_sender();
    vote->request_voting_power(100);
    ctx.flush();

    vote = &ctx.get<Voting>(voting);
    account = &ctx.get<Account>(mrA);
    token = &ctx.get<Token>(band);
    account->set_sender();
    TS_ASSERT_EQUALS(900, token->balance(mrA));
    TS_ASSERT_EQUALS(100, token->balance(voting));
    vote->withdraw_voting_power(50);
    ctx.flush();

    vote = &ctx.get<Voting>(voting);
    account = &ctx.get<Account>(mrA);
    token = &ctx.get<Token>(band);
    account->set_sender();
    TS_ASSERT_EQUALS(950, token->balance(mrA));
  }

  void testCommitVote()
  {
    std::unique_ptr<Storage> store = std::make_unique<StorageMap>();
    Context ctx(*store);
    Global::get().m_ctx = &ctx;
    Global::get().flush = true;

    Address band =
        Address::from_hex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    Curve linear = Curve(std::make_unique<EqVar>());

    {
      ctx.create<Token>(band, band, linear);
    }

    VerifyKey x = VerifyKey::rand();
    Address mrA = ed25519_vk_to_addr(x);

    {
      ctx.create<Account>(mrA, x);
    }

    x = VerifyKey::rand();
    Address mrB = ed25519_vk_to_addr(x);
    {
      ctx.create<Account>(mrB, x);
    }

    Address voting = Address::rand();
    {
      ctx.create<Voting>(voting, band);
    }

    {
      auto account = &ctx.get<Account>(mrA);
      auto token = &ctx.get<Token>(band);
      account->set_sender();
      token->mint(1000);
    }

    {
      auto vote = &ctx.get<Voting>(voting);
      auto account = &ctx.get<Account>(mrA);
      account->set_sender();
      vote->request_voting_power(100);
    }
    uint256_t poll_id;
    {
      auto vote = &ctx.get<Voting>(voting);
      poll_id = vote->start_poll(50, 50, 10, 10);
    }

    {
      auto vote = &ctx.get<Voting>(voting);
      auto account = &ctx.get<Account>(mrA);
      account->set_sender();
      vote->commit_vote(poll_id, get_hash(true, 12), 100);
    }

    // vote = &ctx.get<Voting>(voting);
    // account = &ctx.get<Account>(mrA);
    // token = &ctx.get<Token>(band);
    // account->set_sender();
    // TS_ASSERT_THROWS_ANYTHING(vote->reveal_vote(1, true, 12));
    // ctx.reset();

    // Want to vote more
    {
      auto vote = &ctx.get<Voting>(voting);
      auto account = &ctx.get<Account>(mrA);
      account->set_sender();
      vote->request_voting_power(50);
    }

    {
      auto vote = &ctx.get<Voting>(voting);
      auto account = &ctx.get<Account>(mrA);
      account->set_sender();
      vote->commit_vote(poll_id, get_hash(true, 72), 149);
    }

    // Another account join vote
    {
      auto accountb = &ctx.get<Account>(mrB);
      auto token = &ctx.get<Token>(band);
      accountb->set_sender();
      token->mint(1000);
    }

    {
      auto vote = &ctx.get<Voting>(voting);
      auto accountb = &ctx.get<Account>(mrB);
      accountb->set_sender();
      vote->request_voting_power(365);
    }
    {
      auto vote = &ctx.get<Voting>(voting);
      auto accountb = &ctx.get<Account>(mrB);
      accountb->set_sender();
      vote->commit_vote(poll_id, get_hash(false, 999), 300);
    }

    {
      auto vote = &ctx.get<Voting>(voting);
      auto token = &ctx.get<Token>(band);
      TS_ASSERT_EQUALS(515, token->balance(voting));
      TS_ASSERT(vote->did_commit(mrB, 1));
      TS_ASSERT(!vote->did_commit(band, 1));
    }
  }

  void testRevealVote()
  {
    std::unique_ptr<Storage> store = std::make_unique<StorageMap>();
    Context ctx(*store);
    Global::get().m_ctx = &ctx;
    Global::get().flush = true;
    Global::get().block_time = 0;
    Address band =
        Address::from_hex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    Curve linear = Curve(std::make_unique<EqVar>());
    {
      ctx.create<Token>(band, band, linear);
    }

    VerifyKey x = VerifyKey::rand();
    Address mrA = ed25519_vk_to_addr(x);
    {
      ctx.create<Account>(mrA, x);
    }

    x = VerifyKey::rand();
    Address mrB = ed25519_vk_to_addr(x);
    {
      ctx.create<Account>(mrB, x);
    }

    Address voting = Address::rand();
    {
      ctx.create<Voting>(voting, band);
    }

    {
      auto account = &ctx.get<Account>(mrA);
      auto accountb = &ctx.get<Account>(mrB);
      auto token = &ctx.get<Token>(band);
      account->set_sender();
      token->mint(1000);
      accountb->set_sender();
      token->mint(1000);
    }

    {
      auto vote = &ctx.get<Voting>(voting);
      auto account = &ctx.get<Account>(mrA);
      auto accountb = &ctx.get<Account>(mrB);
      account->set_sender();
      vote->request_voting_power(150);
      accountb->set_sender();
      vote->request_voting_power(365);
    }

    uint256_t poll_id;
    {
      auto vote = &ctx.get<Voting>(voting);
      poll_id = vote->start_poll(50, 50, 10, 10);
    }

    {
      auto vote = &ctx.get<Voting>(voting);
      auto account = &ctx.get<Account>(mrA);
      account->set_sender();
      vote->commit_vote(poll_id, get_hash(true, 72), 149);
    }

    {
      auto vote = &ctx.get<Voting>(voting);
      auto accountb = &ctx.get<Account>(mrB);
      accountb->set_sender();
      vote->commit_vote(1, get_hash(false, 999), 300);
    }

    {
      auto vote = &ctx.get<Voting>(voting);
      auto account = &ctx.get<Account>(mrA);
      account->set_sender();
      TS_ASSERT_THROWS_ANYTHING(vote->reveal_vote(poll_id, true, 12));
      Global::get().flush = false;
    }

    {
      Global::get().block_time = 15;
      Global::get().flush = true;
      auto vote = &ctx.get<Voting>(voting);
      auto account = &ctx.get<Account>(mrA);
      account->set_sender();
      vote->reveal_vote(poll_id, true, 72);
    }

    {
      Global::get().block_time = 17;
      auto vote = &ctx.get<Voting>(voting);
      auto accountb = &ctx.get<Account>(mrB);
      accountb->set_sender();
      vote->reveal_vote(poll_id, false, 999);
    }

    {
      auto vote = &ctx.get<Voting>(voting);
      auto accountb = &ctx.get<Account>(mrB);
      accountb->set_sender();
      vote->withdraw_voting_power(320);
    }

    {
      auto vote = &ctx.get<Voting>(voting);
      auto token = &ctx.get<Token>(band);
      TS_ASSERT_EQUALS(955, token->balance(mrB));
      TS_ASSERT_THROWS_ANYTHING(vote->get_result(poll_id));
      TS_ASSERT(!vote->is_poll_ended(poll_id));
    }
  }

  Hash get_hash(bool vote, const uint256_t& salt) const
  {
    Buffer buf;
    buf << vote << salt;
    return sha256(gsl::make_span(buf.to_raw_string()));
  }
};
