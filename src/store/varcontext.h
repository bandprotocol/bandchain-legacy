#pragma once

#include "inc/essential.h"
#include "store/context.h"
#include "util/iban.h"
#include "util/variable.h"

class VarsContext : public Vars
{
public:
  VarsContext(const uint256_t& _x);

  uint256_t get_x() const final;
  uint256_t get_external_price(const PriceID& key) const final;
  uint256_t get_contract_price(const ContractID& key) const final;

private:
  uint256_t x;
};
