#pragma once

#include "inc/essential.h"
#include "store/context.h"
#include "store/varcontext.h"
#include "util/equation.h"

class CommunityContract
{
public:
  // Create a community contract view from blockchain context and tokenKey
  CommunityContract(Context& _ctx, const ContractID& _contract_id);

  // Get the (readable) equation string of this community contract.
  std::string get_string_equation() const;

  // Set equation to contract and update local curve.
  void set_equation(const Curve& _curve);

  // Get current supply that store along with equation in context in format
  // {current_supply + serialized equation}
  uint256_t get_current_supply() const;

  // Set current supply
  void set_current_supply(uint256_t value);

  uint256_t apply_equation(uint256_t x) const;

private:
  void save(std::pair<uint256_t, Curve> data) const;
  std::pair<uint256_t, Curve> load() const;
  Context& ctx;
  const ContractID& contract_id;

  // Key to context
  const Hash key;
};
