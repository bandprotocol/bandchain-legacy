#pragma once

#include "inc/essential.h"
#include "store/context.h"
#include "util/equation.h"

class CommunityContract
{
public:
  // Create a community contract view from blockchain context and tokenKey
  CommunityContract(Context& _ctx, const TokenKey& _token_key);

  // Get the (readable) equation string of this community contract.
  std::string get_string_equation() const { return curve.to_string(); }

  // Set equation to contract and update local curve.
  void set_equation(const Curve& _curve);

  // Get current supply that store along with equation in context in format
  // {current_supply + serialized equation}
  uint256_t get_current_supply() const { return supply; }

  // Set current supply
  void set_current_supply(uint256_t value);

  uint256_t apply_equation(const Vars& vars) const { return curve.apply(vars); }

private:
  void save() const;
  void load();

  Context& ctx;
  const TokenKey& tokenKey;

  // Key to context
  const Hash key;

  uint256_t supply;
  Curve curve;
};
