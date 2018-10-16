#pragma once

#include <unordered_map>

#include "inc/essential.h"
#include "store/contract.h"
#include "store/upgradable.h"

struct RegistryParameters {
  uint8_t vote_quorum;
  uint8_t dispensation_percentage;
  uint256_t min_deposit;
  uint64_t apply_duration;
  uint64_t commit_duration;
  uint64_t reveal_duration;

  std::string to_string() const
  {
    return "vote_quorum: {}, dispensation_percentage: {}, min_deposit: {}, apply_duration: {}, commit_duration: {}, reveal_duration: {}"_format(
        vote_quorum, dispensation_percentage, min_deposit, apply_duration,
        commit_duration, reveal_duration);
  }
};

inline Buffer& operator>>(Buffer& buf, RegistryParameters& params)
{
  return buf >> params.vote_quorum >> params.dispensation_percentage >>
         params.min_deposit >> params.apply_duration >>
         params.commit_duration >> params.reveal_duration;
}
inline Buffer& operator<<(Buffer& buf, const RegistryParameters& params)
{
  return buf << params.vote_quorum << params.dispensation_percentage
             << params.min_deposit << params.apply_duration
             << params.commit_duration << params.reveal_duration;
}

class Registry : public Contract, public UpgradableImpl<RegistryParameters>
{
public:
  friend class GovernanceTest;

  Registry(const Address& registry_id, const Address& _token_id,
           const Address& _voting_id, const Address& _governance_id,
           uint8_t _vote_quorum, uint8_t _dispensation_percentage,
           const uint256_t& _min_deposit, uint64_t _apply_duration,
           uint64_t _commit_duration, uint64_t _reveal_duration);

  // Callable function
  uint256_t apply(std::string data, uint256_t token_deposit);

  void deposit(uint256_t list_id, uint256_t token_deposit);

  void withdraw(uint256_t list_id, uint256_t token_withdraw);

  void exit(uint256_t list_id);

  uint256_t challenge(uint256_t list_id, std::string data);

  void update_status(uint256_t list_id);

  void claim_reward(uint256_t challenge_id, uint256_t salt);

  // Function for get information
  uint256_t voter_reward(const Address& voter, const uint256_t challenge_id,
                         const uint256_t& salt) const;

  bool can_in_list(const uint256_t& list_id) const;

  bool is_listed(const uint256_t& list_id) const;

  bool has_been_challenge(const uint256_t& list_id) const;

  bool challenge_can_be_resolved(const uint256_t& list_id) const;

  uint256_t determine_reward(const uint256_t& challenge_id) const;

  bool token_claimed(const uint256_t& challenge_id, const Address& voter) const;

  void debug_create() const final;

  void debug_save() const final;

  // Callable query function

  Address get_voting_id() const;

  uint256_t active_list_length() const;

  uint256_t active_list_id_at(uint256_t index) const;

  std::string get_content(uint256_t list_id) const;

  uint256_t get_deposit(uint256_t list_id) const;

  uint256_t get_active_challenge(uint256_t list_id) const;

  bool need_update(uint256_t list_id) const;

  uint256_t get_poll_id(uint256_t challenge_id) const;

  bool is_proposal(uint256_t list_id) const;

  uint64_t get_app_expire(uint256_t list_id) const;

  Address get_list_owner(uint256_t list_id) const;

  Address get_challenger_id(uint256_t challenge_id) const;

  std::string get_reason(uint256_t challenge_id) const;

private:
  void resolve_challenge(const uint256_t& list_id);

  void set_to_list(const uint256_t& list_id);

  void reset_listing(const uint256_t& list_id);

  bool list_exist(const uint256_t& list_id) const;

  bool challenge_exist(const uint256_t& challenge_id) const;

public:
  const Address token_id;
  const Address voting_id;
  const Address governance_id;

private:
  uint256_t listing_nonce = 0;
  uint256_t challenge_nonce = 0;

  struct Listing {
    Listing(uint64_t aet, const Address& _owner, const uint256_t& init_dep,
            const std::string& _data)
        : app_expire_time(aet)
        , owner(_owner)
        , unstake_deposit(init_dep)
        , data(_data)
    {
    }
    const uint64_t app_expire_time;
    bool is_listed = false;
    const Address owner;
    uint256_t unstake_deposit;
    uint256_t challenge_id = 0;
    std::string data;
  };

  struct Challenge {
    Challenge(const uint256_t& pid, const Address& cha,
              const std::string& _data, const uint256_t& reward,
              const uint256_t& _stake)
        : poll_id(pid)
        , challenger(cha)
        , data(_data)
        , remaining_reward_pool(reward)
        , stake(_stake)

    {
    }
    uint256_t poll_id;
    const Address challenger;
    const std::string data;

    uint256_t remaining_reward_pool;
    bool is_resolved = false;
    const uint256_t stake;
    uint256_t remaining_winning_token = 0;
    std::unordered_map<Address, bool> token_claimed;
  };

  std::unordered_map<uint256_t, Challenge> m_challenges;
  std::unordered_map<uint256_t, Listing> m_listings;

  static inline auto log = logger::get("registry");
};
