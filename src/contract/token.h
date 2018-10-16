#pragma once

#include <unordered_map>

#include "inc/essential.h"
#include "store/contract.h"
#include "store/wrapper.h"
#include "store/wrapper_map.h"
#include "util/bytes.h"
#include "util/equation.h"

class Token final : public Contract
{
public:
  Token(const Address& token_id);

  void init(const Address& _base_token_id, const Curve& _buy_curve);

  void mint(uint256_t value);

  void transfer(Address dest, uint256_t value);

  void buy(uint256_t value);

  void sell(uint256_t value);

  uint256_t balance(Address address) const;

  uint256_t spot_price() const;

  uint256_t bulk_price(uint256_t value) const;

public:
  void debug_create() const final;
  void debug_save() const final;

  Wrapper<Address> base_token_id{*this};
  Wrapper<Curve> buy_curve{*this};

private:
  WrapperMap<Address, uint256_t> m_balances{*this};
  Wrapper<uint256_t> current_supply{*this};

  static inline auto log = logger::get("token");
};
