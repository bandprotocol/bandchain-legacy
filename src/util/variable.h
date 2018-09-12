#pragma once

#include "inc/essential.h"

enum class Variable : uint8_t {
  Supply = 1,
  BNDUSD = 2,
  BNDTHB = 3,
  CTPrice = 4,
};

class Vars
{
public:
  virtual ~Vars() {}
  virtual uint256_t get_value(Variable var) const = 0;
};
