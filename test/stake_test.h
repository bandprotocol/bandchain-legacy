#include <array>
#include <cxxtest/TestSuite.h>

#include "contract/account.h"
#include "contract/stake.h"
#include "contract/token.h"
#include "crypto/ed25519.h"
#include "inc/essential.h"
#include "store/context_map.h"
#include "store/global.h"
#include "util/buffer.h"

class StakeTest : public CxxTest::TestSuite
{
public:
  void testCreateStake()
  {
    std::unique_ptr<ContextMap> ctx = std::make_unique<ContextMap>();
    Global::get().m_ctx = ctx.get();

    Address mrA = createAccount();

    Address band =
        Address::from_hex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    Curve linear = Curve(std::make_unique<EqVar>());
    ctx->create<Token>(band, band, linear);

    Address stake_id = Address::rand();
    ctx->create<Stake>(stake_id, band);
    ctx->flush();

    mint_band(mrA, 1000);

    auto account = &ctx->get<Account>(mrA);
    auto stake = &ctx->get<Stake>(stake_id);

    account->set_sender();
    stake->create_party(100, 2, 100);
    TS_ASSERT_EQUALS(mrA, stake->m_parties[1].leader);
    TS_ASSERT_EQUALS(1, stake->m_parties.size());
    ctx->flush();
  }

  void testStake()
  {
    std::unique_ptr<ContextMap> ctx = std::make_unique<ContextMap>();
    Global::get().m_ctx = ctx.get();
    Global::get().block_time = 0;

    Address band =
        Address::from_hex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    Curve linear = Curve(std::make_unique<EqVar>());

    Address mrA = createAccount();

    ctx->create<Token>(band, band, linear);
    Address stake_id = Address::rand();
    ctx->create<Stake>(stake_id, band);
    ctx->flush();

    mint_band(mrA, 1000);

    auto account = &ctx->get<Account>(mrA);
    auto token = &ctx->get<Token>(band);
    auto stake = &ctx->get<Stake>(stake_id);

    account->set_sender();
    stake->create_party(100, 2, 100);

    ctx->flush();

    Address mrB = createAccount();
    mint_band(mrB, 777);

    auto account_b = &ctx->get<Account>(mrB);
    stake = &ctx->get<Stake>(stake_id);
    account = &ctx->get<Account>(mrA);
    token = &ctx->get<Token>(band);
    account_b->set_sender();
    stake->stake(1, 500);
    ctx->flush();

    account_b = &ctx->get<Account>(mrB);
    stake = &ctx->get<Stake>(stake_id);
    account = &ctx->get<Account>(mrA);
    token = &ctx->get<Token>(band);
    TS_ASSERT_EQUALS(600, stake->m_parties[1].current_stake);
    TS_ASSERT_EQUALS(2, stake->receipt_nonce);
    TS_ASSERT_EQUALS(1, stake->m_receipts[2].party_id);
    TS_ASSERT_EQUALS(mrB, stake->m_receipts[2].owner);
    TS_ASSERT_EQUALS(500, stake->m_receipts[2].value);
    TS_ASSERT_EQUALS(277, token->balance(mrB));
    TS_ASSERT_EQUALS(600, token->balance(stake_id));

    // Create another party
    Address leader_2 = createAccount();
    Address p2_1 = createAccount();
    Address p2_2 = createAccount();

    mint_band(leader_2, 400);
    mint_band(p2_1, 200);
    mint_band(p2_2, 200);

    auto account_l2 = &ctx->get<Account>(leader_2);
    auto account_p2_1 = &ctx->get<Account>(p2_1);
    auto account_p2_2 = &ctx->get<Account>(p2_2);
    token = &ctx->get<Token>(band);
    stake = &ctx->get<Stake>(stake_id);
    account_l2->set_sender();
    stake->create_party(350, 3, 1000);
    ctx->flush();

    account_l2 = &ctx->get<Account>(leader_2);
    account_p2_1 = &ctx->get<Account>(p2_1);
    account_p2_2 = &ctx->get<Account>(p2_2);
    token = &ctx->get<Token>(band);
    stake = &ctx->get<Stake>(stake_id);
    account_p2_1->set_sender();
    stake->stake(2, 200);
    account_p2_2->set_sender();
    stake->stake(2, 100);
    ctx->flush();

    account_l2 = &ctx->get<Account>(leader_2);
    account_p2_1 = &ctx->get<Account>(p2_1);
    account_p2_2 = &ctx->get<Account>(p2_2);
    token = &ctx->get<Token>(band);
    stake = &ctx->get<Stake>(stake_id);
    TS_ASSERT_EQUALS(650, stake->m_parties[2].current_stake);
    TS_ASSERT_EQUALS(5, stake->receipt_nonce);
    TS_ASSERT_EQUALS(2, stake->m_receipts[3].party_id);
    TS_ASSERT_EQUALS(leader_2, stake->m_receipts[3].owner);
    TS_ASSERT_EQUALS(350, stake->m_receipts[3].value);
    TS_ASSERT_EQUALS(50, token->balance(leader_2));

    TS_ASSERT_EQUALS(2, stake->m_receipts[4].party_id);
    TS_ASSERT_EQUALS(p2_1, stake->m_receipts[4].owner);
    TS_ASSERT_EQUALS(200, stake->m_receipts[4].value);
    TS_ASSERT_EQUALS(0, token->balance(p2_1));

    TS_ASSERT_EQUALS(2, stake->m_receipts[5].party_id);
    TS_ASSERT_EQUALS(p2_2, stake->m_receipts[5].owner);
    TS_ASSERT_EQUALS(100, stake->m_receipts[5].value);
    TS_ASSERT_EQUALS(100, token->balance(p2_2));

    TS_ASSERT_EQUALS(1250, token->balance(stake_id));
  }

  void testAddRewardAndClaimReward()
  {
    std::unique_ptr<ContextMap> ctx = std::make_unique<ContextMap>();
    Global::get().m_ctx = ctx.get();
    Global::get().block_time = 0;

    Address band =
        Address::from_hex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    Curve linear = Curve(std::make_unique<EqVar>());

    Address mrA = createAccount();

    ctx->create<Token>(band, band, linear);
    Address stake_id = Address::rand();
    ctx->create<Stake>(stake_id, band);
    ctx->flush();

    auto token = &ctx->get<Token>(band);
    auto stake = &ctx->get<Stake>(stake_id);
    auto account = &ctx->get<Account>(mrA);
    std::array<Address, 10> participants;

    for (auto& x : participants) {
      x = createAccount();
      mint_band(x, 300);
    }

    mint_band(mrA, 1000);

    stake = &ctx->get<Stake>(stake_id);
    account = &ctx->get<Account>(mrA);
    token = &ctx->get<Token>(band);
    account->set_sender();
    stake->create_party(200, 1, 1000);
    ctx->flush();

    stake = &ctx->get<Stake>(stake_id);
    token = &ctx->get<Token>(band);
    for (auto& x : participants) {
      auto& account = ctx->get<Account>(x);
      account.set_sender();
      stake->stake(1, 200);
    }
    ctx->flush();

    Global::get().block_time = 100;

    stake = &ctx->get<Stake>(stake_id);
    token = &ctx->get<Token>(band);
    stake->add_reward(1, 1000);
    ctx->flush();

    stake = &ctx->get<Stake>(stake_id);
    account = &ctx->get<Account>(mrA);
    token = &ctx->get<Token>(band);

    TS_ASSERT_EQUALS(1800, token->balance(mrA));
    TS_ASSERT_EQUALS(100, stake->m_parties[1].sum_reward[0].first);
    TS_ASSERT_EQUALS(0, stake->m_parties[1].sum_reward[0].second);

    Global::get().block_time = 150;

    // Add 2 participants
    // pc_2
    std::array<Address, 2> new_pc;
    for (auto& x : new_pc) {
      x = createAccount();
      mint_band(x, 555);
      auto& t_account = ctx->get<Account>(x);
      auto& t_stake = ctx->get<Stake>(stake_id);
      t_account.set_sender();
      t_stake.stake(1, 300);
      ctx->flush();
    }

    TS_ASSERT_EQUALS(2800, stake->m_parties[1].current_stake);
    TS_ASSERT_EQUALS(2200, stake->m_parties[1].last_checkpoint_stake);

    // Old staker out
    auto& tmp_acc = ctx->get<Account>(participants[3]);
    stake = &ctx->get<Stake>(stake_id);
    token = &ctx->get<Token>(band);

    tmp_acc.set_sender();
    stake->withdraw(5);

    TS_ASSERT_EQUALS(2600, stake->m_parties[1].current_stake);
    TS_ASSERT_EQUALS(2000, stake->m_parties[1].last_checkpoint_stake);

    Global::get().block_time = 400;
    // New staker give up
    auto& tmp_na = ctx->get<Account>(new_pc[0]);
    tmp_na.set_sender();
    TS_ASSERT_THROWS_ANYTHING(stake->withdraw(11));
    stake->withdraw(12);

    TS_ASSERT_EQUALS(2300, stake->m_parties[1].current_stake);
    TS_ASSERT_EQUALS(2000, stake->m_parties[1].last_checkpoint_stake);
    ctx->flush();
    // Reward comingggg
    Global::get().block_time = 500;

    stake = &ctx->get<Stake>(stake_id);
    token = &ctx->get<Token>(band);
    stake->add_reward(1, 3000);
    ctx->flush();

    stake = &ctx->get<Stake>(stake_id);
    token = &ctx->get<Token>(band);
    TS_ASSERT_EQUALS(1803, token->balance(mrA));
    uint256_t ans{"149850000000000000000000000000000000"};
    TS_ASSERT_EQUALS(500, stake->m_parties[1].sum_reward[1].first);
    TS_ASSERT_EQUALS(ans, stake->m_parties[1].sum_reward[1].second);

    // Claim reward
    Global::get().block_time = 600;
    TS_ASSERT_EQUALS(100, token->balance(participants[2]));
    TS_ASSERT_EQUALS(0, stake->m_receipts[4].last_update_time);
    auto temp_acc = &ctx->get<Account>(participants[2]);
    temp_acc->set_sender();
    stake->claim_reward(4);
    ctx->flush();

    stake = &ctx->get<Stake>(stake_id);
    account = &ctx->get<Account>(mrA);
    token = &ctx->get<Token>(band);

    TS_ASSERT_EQUALS(399, token->balance(participants[2]));
    TS_ASSERT_EQUALS(500, stake->m_receipts[4].last_update_time);

    stake = &ctx->get<Stake>(stake_id);
    token = &ctx->get<Token>(band);
    temp_acc = &ctx->get<Account>(new_pc[1]);
    temp_acc->set_sender();
    stake->claim_reward(13);
    ctx->flush();

    stake = &ctx->get<Stake>(stake_id);
    account = &ctx->get<Account>(mrA);
    token = &ctx->get<Token>(band);

    // Don't receive reward
    TS_ASSERT_EQUALS(255, token->balance(new_pc[1]));
    TS_ASSERT_EQUALS(500, stake->m_receipts[13].last_update_time);

    Global::get().block_time = 800;

    stake = &ctx->get<Stake>(stake_id);
    token = &ctx->get<Token>(band);
    stake->add_reward(1, 5000);
    ctx->flush();

    stake = &ctx->get<Stake>(stake_id);
    token = &ctx->get<Token>(band);
    TS_ASSERT_EQUALS(1808, token->balance(mrA));
    uint256_t ans2{"367023913043478260869565217391304347"};
    TS_ASSERT_EQUALS(800, stake->m_parties[1].sum_reward[2].first);
    TS_ASSERT_EQUALS(ans2, stake->m_parties[1].sum_reward[2].second);

    // Claim reward
    Global::get().block_time = 870;
    TS_ASSERT_EQUALS(100, token->balance(participants[0]));
    TS_ASSERT_EQUALS(0, stake->m_receipts[2].last_update_time);
    temp_acc = &ctx->get<Account>(participants[0]);
    temp_acc->set_sender();
    stake->claim_reward(2);
    ctx->flush();

    stake = &ctx->get<Stake>(stake_id);
    account = &ctx->get<Account>(mrA);
    token = &ctx->get<Token>(band);

    TS_ASSERT_EQUALS(834, token->balance(participants[0]));
    TS_ASSERT_EQUALS(800, stake->m_receipts[2].last_update_time);

    stake = &ctx->get<Stake>(stake_id);
    token = &ctx->get<Token>(band);
    temp_acc = &ctx->get<Account>(new_pc[1]);
    temp_acc->set_sender();
    stake->claim_reward(13);
    ctx->flush();

    stake = &ctx->get<Stake>(stake_id);
    account = &ctx->get<Account>(mrA);
    token = &ctx->get<Token>(band);

    // Don't receive reward
    TS_ASSERT_EQUALS(255 + 651, token->balance(new_pc[1]));
    TS_ASSERT_EQUALS(800, stake->m_receipts[13].last_update_time);
  }

  void testWithdraw()
  {
    std::unique_ptr<ContextMap> ctx = std::make_unique<ContextMap>();
    Global::get().m_ctx = ctx.get();
    Global::get().block_time = 0;

    Address band =
        Address::from_hex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    Curve linear = Curve(std::make_unique<EqVar>());

    VerifyKey x = VerifyKey::rand();
    Address mrA = ed25519_vk_to_addr(x);
    ctx->create<Account>(x);

    ctx->create<Token>(band, band, linear);
    Address stake_id = Address::rand();
    ctx->create<Stake>(stake_id, band);
    ctx->flush();

    auto token = &ctx->get<Token>(band);
    auto stake = &ctx->get<Stake>(stake_id);
    std::array<Address, 10> participants;
    for (auto& x : participants) {
      x = createAccount();
      mint_band(x, 300);
    }

    mint_band(mrA, 1000);

    stake = &ctx->get<Stake>(stake_id);
    auto account = &ctx->get<Account>(mrA);
    token = &ctx->get<Token>(band);
    account->set_sender();
    stake->create_party(200, 1, 1000);
    ctx->flush();

    stake = &ctx->get<Stake>(stake_id);
    token = &ctx->get<Token>(band);
    for (auto& x : participants) {
      auto& account = ctx->get<Account>(x);
      account.set_sender();
      stake->stake(1, 200);
    }
    ctx->flush();

    stake = &ctx->get<Stake>(stake_id);
    token = &ctx->get<Token>(band);
    TS_ASSERT_EQUALS(2200, stake->m_parties[1].current_stake);

    auto temp_acc = ctx->get<Account>(participants[5]);
    temp_acc.set_sender();

    TS_ASSERT_EQUALS(100, token->balance(participants[5]));
    stake->withdraw(7);
    TS_ASSERT_EQUALS(300, token->balance(participants[5]));
    TS_ASSERT_EQUALS(2000, stake->m_parties[1].current_stake);

    // // Another withdraw between reward test in addreward/ claimreward already
  }

  void testTopx()
  {
    std::unique_ptr<ContextMap> ctx = std::make_unique<ContextMap>();

    Global::get().m_ctx = ctx.get();

    Address band =
        Address::from_hex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    Curve linear = Curve(std::make_unique<EqVar>());

    ctx->create<Token>(band, band, linear);
    Address stake_id = Address::rand();
    ctx->create<Stake>(stake_id, band);
    ctx->flush();

    auto stake = &ctx->get<Stake>(stake_id);
    std::array<Address, 10> leaders;

    uint256_t i = 100;
    for (auto& x : leaders) {
      x = createAccount();
      mint_band(x, 2000);
      auto acc = ctx->get<Account>(x);
      acc.set_sender();
      stake->create_party(i, 1, 1000);
      i += 100;
    }

    ctx->flush();
    stake = &ctx->get<Stake>(stake_id);
    auto top3 = stake->topx(3);

    TS_ASSERT_EQUALS(3, top3.size());
    for (int ii = 0; ii < 3; ++ii) {
      TS_ASSERT_EQUALS(leaders[9 - ii], top3[ii]);
    }

    // Stake more on party 4
    auto tmp_acc = &ctx->get<Account>(leaders[3]);
    tmp_acc->set_sender();
    stake->stake(4, 1500);
    ctx->flush();

    stake = &ctx->get<Stake>(stake_id);
    auto top5 = stake->topx(5);
    TS_ASSERT_EQUALS(leaders[3], top5[0]);
    for (int ii = 0; ii < 4; ++ii) {
      TS_ASSERT_EQUALS(leaders[9 - ii], top5[ii + 1]);
    }
  }

  void testActivateAndDestroy()
  {
    std::unique_ptr<ContextMap> ctx = std::make_unique<ContextMap>();

    Global::get().m_ctx = ctx.get();

    Address band =
        Address::from_hex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    Curve linear = Curve(std::make_unique<EqVar>());

    ctx->create<Token>(band, band, linear);
    Address stake_id = Address::rand();
    ctx->create<Stake>(stake_id, band);
    ctx->flush();

    auto stake = &ctx->get<Stake>(stake_id);
    std::array<Address, 10> leaders;

    uint256_t i = 100;
    for (auto& x : leaders) {
      x = createAccount();
      mint_band(x, 2000);
      auto acc = ctx->get<Account>(x);
      acc.set_sender();
      stake->create_party(i, 1, 1000);
      i += 100;
    }

    ctx->flush();

    stake = &ctx->get<Stake>(stake_id);
    stake->deactivate_party(9);
    ctx->flush();
    stake = &ctx->get<Stake>(stake_id);
    auto top3 = stake->topx(3);

    TS_ASSERT_EQUALS(leaders[9], top3[0]);
    TS_ASSERT_EQUALS(leaders[7], top3[1]);
    TS_ASSERT_EQUALS(leaders[6], top3[2]);

    stake = &ctx->get<Stake>(stake_id);
    auto tmp_acc = &ctx->get<Account>(leaders[8]);
    tmp_acc->set_sender();
    stake->withdraw(9);
    ctx->flush();

    stake = &ctx->get<Stake>(stake_id);
    tmp_acc = &ctx->get<Account>(leaders[8]);
    tmp_acc->set_sender();
    stake->stake(9, 1500);
    ctx->flush();

    stake = &ctx->get<Stake>(stake_id);
    top3 = stake->topx(3);

    TS_ASSERT_EQUALS(leaders[9], top3[0]);
    TS_ASSERT_EQUALS(leaders[7], top3[1]);
    TS_ASSERT_EQUALS(leaders[6], top3[2]);

    tmp_acc->set_sender();
    stake->reactivate_party(9);
    ctx->flush();

    stake = &ctx->get<Stake>(stake_id);
    top3 = stake->topx(3);

    TS_ASSERT_EQUALS(leaders[8], top3[0]);
    TS_ASSERT_EQUALS(leaders[9], top3[1]);
    TS_ASSERT_EQUALS(leaders[7], top3[2]);

    stake->destroy_party(8);
    ctx->flush();

    stake = &ctx->get<Stake>(stake_id);
    top3 = stake->topx(3);

    TS_ASSERT_EQUALS(leaders[8], top3[0]);
    TS_ASSERT_EQUALS(leaders[9], top3[1]);
    TS_ASSERT_EQUALS(leaders[6], top3[2]);

    tmp_acc = &ctx->get<Account>(leaders[7]);
    tmp_acc->set_sender();
    TS_ASSERT_THROWS_ANYTHING(stake->withdraw(8));
    ctx->flush();
  }

  Address createAccount(VerifyKey vk = VerifyKey::rand())
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
