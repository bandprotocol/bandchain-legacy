#pragma once

#include "inc/essential.h"

class Vars
{
public:
  virtual ~Vars() {}
  virtual uint256_t get_x() const = 0;
  virtual uint256_t get_external_price(const ContextKey& key) const = 0;
  virtual uint256_t get_contract_price(const ContextKey& key) const = 0;
};
