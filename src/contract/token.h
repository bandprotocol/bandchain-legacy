#pragma once

#include <unordered_map>

#include "inc/essential.h"
#include "store/contract.h"
#include "store/data.h"
#include "store/mapping.h"
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

  Data<Address> base_token_id{sha256(m_addr, uint16_t(1))};
  Data<Curve> buy_curve{sha256(m_addr, uint16_t(2))};

private:
  Mapping<Address, Data<uint256_t>> m_balances{sha256(m_addr, uint16_t(3))};
  Data<uint256_t> current_supply{sha256(m_addr, uint16_t(4))};

  static inline auto log = logger::get("token");
};
