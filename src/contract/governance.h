#pragma once

#include <unordered_map>

#include "contract/tcr.h"
#include "inc/essential.h"
#include "store/contract.h"
#include "store/upgradable.h"

struct GovernanceParameters {
  uint8_t losing_threshold;
  uint8_t winning_threshold;
  uint256_t min_deposit;
  uint64_t commit_duration;
  uint64_t reveal_duration;

  std::string to_string() const
  {
    return "losing_threshold: {}, winning_threshold: {}, min_deposit: {}, commit_duration: {}, reveal_duration: {}"_format(
        losing_threshold, winning_threshold, min_deposit, commit_duration,
        reveal_duration);
  }
};

inline Buffer& operator>>(Buffer& buf, GovernanceParameters& params)
{
  return buf >> params.losing_threshold >> params.winning_threshold >>
         params.min_deposit >> params.commit_duration >> params.reveal_duration;
}
inline Buffer& operator<<(Buffer& buf, const GovernanceParameters& params)
{
  return buf << params.losing_threshold << params.winning_threshold
             << params.min_deposit << params.commit_duration
             << params.reveal_duration;
}

class Governance : public Contract, public UpgradableImpl<GovernanceParameters>
{
public:
  friend class GovernanceTest;
  Governance(const Address& governance_id, const Address& _token_id,
             const Address& _voting_id, uint8_t _losing_threshold,
             uint8_t _winning_threshold, const uint256_t& _min_deposit,
             uint64_t _commit_duration, uint64_t _reveal_duration);

  // Callable function
  uint256_t propose_new_parameter(Address contract_id, Buffer buffer);

  // uint256_t propose_governance_parameter(Buffer buffer);

  void resolve_proposal(uint256_t proposal_id);

  void debug_create() const final;

  void debug_save() const final;

public:
  const Address token_id;
  const Address voting_id;

private:
  uint256_t nonce_proposal = 0;

  struct Proposal {
    const uint256_t poll_id;
    const Address contract_id;
    const Address proposer;
    const Buffer data;
    const std::string detail;
    bool is_resolved;
  };

  std::unordered_map<uint256_t, Proposal> m_proposals;
  std::unordered_map<Address, bool> in_progress;

  static inline auto log = logger::get("governance");
};
