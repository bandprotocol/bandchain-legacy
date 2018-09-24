#pragma once

#include "inc/essential.h"
#include "util/iban.h"

class Vars
{
public:
  virtual ~Vars() {}

  virtual uint256_t get_x() const = 0;
  virtual uint256_t get_external_price(const Address& key) const = 0;
  virtual uint256_t get_contract_price(const Address& key) const = 0;
};

class VarsSimple : public Vars
{
public:
  VarsSimple(uint256_t _s)
      : s(_s)
  {
  }

  uint256_t get_x() const final { return s; }
  uint256_t get_external_price(const Address& key) const
  {
    throw Error("External price isn't supported now");
  }
  uint256_t get_contract_price(const Address& key) const
  {
    throw Error("Contract price isn't supported now");
  }

  void set_s(const uint256_t& value) { s = value; }

private:
  uint256_t s;
};
