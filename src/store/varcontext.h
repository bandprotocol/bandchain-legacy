#pragma once

#include "inc/essential.h"
#include "store/context.h"
#include "util/bytes.h"
#include "util/variable.h"

class VarsContext : public Vars
{
public:
  VarsContext(uint256_t _x);
  uint256_t get_x() const final;
  uint256_t get_external_price(const ContextKey& key) const final;
  uint256_t get_contract_price(const ContextKey& key) const final;

private:
  uint256_t x;
};

// Variable context for product token contract
// class VarsContextPT : public VarsContext
// {
// public:
//   VarsContextPT(Context& _ctx, uint256_t _supply, uint256_t _ct_price);
//   uint256_t get_value(Variable var) const final;
//   void set_ct_price(uint256_t _ct_price) { ct_price = _ct_price; }

// private:
//   uint256_t ct_price;
// };
