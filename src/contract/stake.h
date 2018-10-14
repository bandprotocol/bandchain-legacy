#pragma once

#include <set>
#include <unordered_map>
#include <vector>

#include "inc/essential.h"
#include "store/contract.h"

class Stake : public Contract
{
public:
  friend class StakeTest;
  Stake(const Address& stake_id, const Address& _base_token);

  ContractID contract_id() const final { return ContractID::Stake; }

  // Callable functions
  // Stake token to dest (Validator) and create a new receipt.
  uint256_t stake(uint256_t party_id, uint256_t value);

  // Withdraw all token from receipt and delete that receipt.
  void withdraw(uint256_t receipt_id);

  // Create new party and stake some token to it.
  uint256_t create_party(uint256_t value, uint256_t numerator,
                         uint256_t denominator);

  // Sender get reward from token that they stake from stark stake or last
  // claim_reward until now.
  void claim_reward(uint256_t receipt_id);

  void reactivate_party(uint256_t party_id);

  // Query
  std::vector<Address> topx(uint16_t value) const;

  // Add reward from block that dest proposed block. (Store for claiming reward)
  void add_reward(uint256_t party_id, uint256_t value);

  // Deactivate party that didn't voted
  void deactivate_party(uint256_t party_id);

  void destroy_party(uint256_t party_id);

  std::unique_ptr<Contract> clone() const final;
  void debug_create() const final;
  void debug_save() const final;

  const Address base_token;

private:
  struct Receipt {
    uint64_t last_update_time;
    const Address owner;
    const uint256_t party_id;
    const uint256_t value;
  };

  struct Party {
    uint256_t last_checkpoint_stake;
    uint256_t current_stake;
    const Address leader;
    const uint256_t numerator;
    const uint256_t denominator;
    bool is_active;
    std::vector<std::pair<uint64_t, uint256_t>> sum_reward;
  };

  uint256_t receipt_nonce = 0;
  uint256_t party_nonce = 0;

  const uint256_t magnitude{"100000000000000000000000000000000000"};

  std::unordered_map<uint256_t, Receipt> m_receipts;
  std::unordered_map<uint256_t, Party> m_parties;
  std::set<std::pair<uint256_t, uint256_t>> active_party_list;

  static inline auto log = logger::get("stake");
};
