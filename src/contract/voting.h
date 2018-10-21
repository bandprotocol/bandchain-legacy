#pragma once

#include <set>
#include <unordered_map>

#include "inc/essential.h"
#include "store/contract.h"
#include "store/data.h"
#include "store/mapping.h"
#include "store/set.h"

BETTER_ENUM(VotingStatus, uint8_t, Committed = 1, Revealed = 2)

BETTER_ENUM(PollStatus, uint8_t, Commit = 0, Reveal = 1, End = 2)

BETTER_ENUM(VoteResult, uint8_t, Reject = 0, Approve = 1, NoVote = 2)

class Voting : public Contract
{
public:
  friend class VotingTest;
  friend class GovernanceTest;

  Voting(const Address& voting_id);

  void init(const Address& _token_id);

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

  void debug_create() const final;

  void debug_save() const final;

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
  Data<Address> token_id{sha256(m_addr, uint16_t(1))};
  Data<uint256_t> poll_nonce{sha256(m_addr, uint16_t(2))};

private:
  Mapping<Address, Data<uint256_t>> voting_power{sha256(m_addr, uint16_t(3))};

  struct PollReceipt {

    PollReceipt(const Hash& _parent_hash)
        : parent_hash(_parent_hash)
    {
    }

    void init(const uint256_t& _num_tokens, const Hash& _commit_hash)

    {
      _exist = true;
      status = VotingStatus::Committed;
      num_tokens = _num_tokens;
      commit_hash = _commit_hash;
    }

    void erase()
    {
      _exist.erase();
      status.erase();
      num_tokens.erase();
      commit_hash.erase();
    }

    bool exist() { return _exist.exist(); }

    const Hash parent_hash;

    Data<bool> _exist{parent_hash};
    Data<VotingStatus> status{sha256(parent_hash, uint16_t(1))};
    Data<uint256_t> num_tokens{sha256(parent_hash, uint16_t(2))};
    Data<Hash> commit_hash{sha256(parent_hash, uint16_t(3))};
  };
  struct Poll {

    Poll(const Hash& _parent_hash)
        : parent_hash(_parent_hash)
    {
    }

    void init(uint64_t _commit_end_time, uint64_t _reveal_end_time,
              uint8_t _losing_threshold, uint8_t _winning_threshold)
    {
      _exist = true;
      commit_end_time = _commit_end_time;
      reveal_end_time = _reveal_end_time;
      losing_threshold = _losing_threshold;
      winning_threshold = _winning_threshold;
      votes_for = 0;
      votes_against = 0;
    }

    void erase()
    {
      _exist.erase();
      commit_end_time.erase();
      reveal_end_time.erase();
      losing_threshold.erase();
      winning_threshold.erase();
      votes_for.erase();
      votes_against.erase();
    }

    bool exist() { return _exist.exist(); }

    const Hash parent_hash;

    Data<bool> _exist{parent_hash};
    Data<uint64_t> commit_end_time{sha256(parent_hash, uint16_t(1))};
    Data<uint64_t> reveal_end_time{sha256(parent_hash, uint16_t(2))};
    Data<uint8_t> losing_threshold{sha256(parent_hash, uint16_t(3))};
    Data<uint8_t> winning_threshold{sha256(parent_hash, uint16_t(4))};
    Data<uint256_t> votes_for{sha256(parent_hash, uint16_t(5))};
    Data<uint256_t> votes_against{sha256(parent_hash, uint16_t(6))};
    Mapping<Address, PollReceipt> votes{sha256(parent_hash, uint16_t(7))};
  };

  Mapping<uint256_t, Poll> m_poll{sha256(m_addr, uint16_t(4))};
  Mapping<Address, Set<std::pair<uint256_t, uint256_t>>> poll_list{
      sha256(m_addr, uint16_t(5))};
  static inline auto log = logger::get("voting");
};
