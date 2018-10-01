#pragma once

#include <unordered_map>

#include "inc/essential.h"
#include "store/contract.h"
#include "util/bytes.h"
#include "util/equation.h"

class Token final : public Contract
{
public:
  Token(const Address& token_id, const Address& _base_token_id,
        const Curve& _buy_curve);

  void mint(uint256_t value);

  void transfer(Address dest, uint256_t value);

  void buy(uint256_t value);

  void sell(uint256_t value);

  uint256_t balance(Address address) const;

  uint256_t spot_price() const;

  uint256_t bulk_price(uint256_t value) const;

public:
  ContractID contract_id() const final { return ContractID::Token; }

  void debug_create() const final;
  void debug_save() const final;
  std::unique_ptr<Contract> clone() const final
  {
    return std::make_unique<Token>(*this);
  }

  const Address base_token_id;
  const Curve buy_curve;

private:
  std::unordered_map<Address, uint256_t> m_balances;
  uint256_t current_supply = 0;

  static inline auto log = logger::get("token");
};
