#pragma once

#include <set>
#include <unordered_map>

#include "inc/essential.h"
#include "store/contract.h"

BETTER_ENUM(VotingStatus, uint8_t, Committed = 1, Revealed = 2)

BETTER_ENUM(PollStatus, uint8_t, Commit = 0, Reveal = 1, End = 2)

BETTER_ENUM(VoteResult, uint8_t, Reject = 0, Approve = 1, NoVote = 2)

class Voting : public Contract
{
public:
  friend class VotingTest;
  friend class GovernanceTest;

  Voting(const Address& voting_id, const Address& _token_id);

  // Callable action function

  void request_voting_power(uint256_t value);

  void withdraw_voting_power(uint256_t value);

  void rescue_token(uint256_t poll_id);

  void commit_vote(uint256_t poll_id, Hash secret_hash, uint256_t num_tokens);

  void reveal_vote(uint256_t poll_id, bool vote_option, uint256_t salt);

  // Callable query function

  uint256_t get_vote_for(uint256_t poll_id) const;

  uint256_t get_vote_against(uint256_t poll_id) const;

  uint8_t get_period(uint256_t poll_id) const;

  uint8_t get_result(uint256_t poll_id) const;

  uint64_t get_commit_end_time(uint256_t poll_id) const;

  uint64_t get_reveal_end_time(uint256_t poll_id) const;

  uint256_t get_voting_power(Address addr) const;

  // Call by tcr function

  uint256_t get_number_pass_token(const Address& address,
                                  const uint256_t& poll_id,
                                  const uint256_t& salt) const;

  uint256_t start_poll(uint8_t losing_threshold, uint8_t winning_threahold,
                       uint64_t commit_duration, uint64_t reveal_duration);

  uint256_t get_total_winning_token(const uint256_t& poll_id) const;

  bool is_poll_ended(const uint256_t& poll_id) const;

  ContractID contract_id() const final { return ContractID::Voting; }

  void debug_create() const final;

  void debug_save() const final;

  std::unique_ptr<Contract> clone() const final;

private:
  bool is_commit_period(const uint256_t& poll_id) const;

  bool is_reveal_period(const uint256_t& poll_id) const;

  bool did_commit(const Address& address, const uint256_t& poll_id) const;

  bool did_reveal(const Address& address, const uint256_t& poll_id) const;

  void assert_poll_exists(const uint256_t& poll_id) const;

  void assert_vote_exist(const Address& address,
                         const uint256_t& poll_id) const;

  Hash get_commit_hash(const Address& address, const uint256_t& poll_id) const;

  uint256_t get_num_tokens(const Address& address,
                           const uint256_t& poll_id) const;

  uint256_t get_maximum_vote(const Address& address) const;

  bool is_expired(uint64_t terminate_time) const;

  Hash get_hash(bool vote, const uint256_t& salt) const;

public:
  const Address token_id;
  uint256_t poll_nonce = 0;

private:
  std::unordered_map<Address, uint256_t> voting_power;

  struct PollReceipt {
    PollReceipt(const uint256_t& nt, const Hash& ch)
        : status(VotingStatus::Committed)
        , num_tokens(nt)
        , commit_hash(ch)
    {
    }
    VotingStatus status = VotingStatus::Committed;
    const uint256_t num_tokens;
    const Hash commit_hash;
  };
  struct Poll {
    Poll(uint64_t cet, uint64_t ret, uint8_t lt, uint8_t wt)
        : commit_end_time(cet)
        , reveal_end_time(ret)
        , losing_threshold(lt)
        , winning_threshold(wt)
    {
    }
    const uint64_t commit_end_time;
    const uint64_t reveal_end_time;
    const uint8_t losing_threshold;
    const uint8_t winning_threshold;
    uint256_t votes_for = 0;
    uint256_t votes_against = 0;
    std::unordered_map<Address, PollReceipt> votes;
  };

  std::unordered_map<uint256_t, Poll> m_poll;
  std::unordered_map<Address, std::set<std::pair<uint256_t, uint256_t>>>
      poll_list;
  static inline auto log = logger::get("voting");
};
