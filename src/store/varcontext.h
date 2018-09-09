#pragma once

#include "inc/essential.h"
#include "store/context.h"
#include "util/variable.h"

class VarsContext : public Vars
{
public:
  VarsContext(Context& _ctx, uint256_t _supply);
  uint256_t get_value(Variable var) const final;
  void set_supply(uint256_t _supply) { supply = _supply; }

private:
  Context& ctx;
  uint256_t supply;
};
