#pragma once

#include <unordered_map>

#include "inc/essential.h"
#include "store/contract.h"
#include "util/bytes.h"

class Token final : public Contract
{
public:
  Token();

  void mint(uint256_t value);

  void transfer(Address dest, uint256_t value);

public:
  void debug_create() const final;
  void debug_save() const final;
  std::unique_ptr<Contract> clone() const final
  {
    return std::make_unique<Token>(*this);
  }

private:
  std::unordered_map<Address, uint256_t> m_balances;
};
