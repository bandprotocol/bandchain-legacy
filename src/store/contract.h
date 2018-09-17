#pragma once

#include "inc/essential.h"
#include "store/context.h"
#include "store/varcontext.h"
#include "util/equation.h"

class Contract : public Object
{
public:
  Contract(const ContractID& contract_id, const ContextKey& _revenue_id,
           const Curve& _curve, const uint256_t& _max_supply,
           bool _is_transferable, bool _is_discountable,
           const Address& _beneficiary);

  Contract(const Contract& contract);

  std::string get_string_equation() const;

  PriceSpread get_price_spread() const;

  uint256_t get_buy_price(const uint256_t& token_supply) const;

  uint256_t get_sell_price(const uint256_t& token_supply) const;

  void debug_create() const;
  void debug_save() const;

public:
  const ContextKey revenue_id;
  const Address beneficiary;
  const bool is_transferable;
  const bool is_discountable;
  const uint256_t max_supply;

  uint256_t circulating_supply = 0;
  uint256_t total_supply = 0;

private:
  const Curve curve;

  static inline auto log = logger::get("contract");
};
