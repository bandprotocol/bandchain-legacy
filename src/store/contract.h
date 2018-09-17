#pragma once

#include "inc/essential.h"
#include "store/context.h"
#include "store/varcontext.h"
#include "util/equation.h"

class Contract : public Object
{
public:
  // Create a community contract view from blockchain context and tokenKey
  Contract(const ContractID& contract_id, const ContextKey& _revenue_id,
           const Curve& _curve, const uint256_t& _max_supply,
           uint8_t _is_transferable, uint8_t _is_discountable,
           const Address& _beneficiary);

  Contract(const Contract& contract);

  ContextKey get_revenue_id() const;

  // Get the (readable) equation string of this community contract.
  std::string get_string_equation() const;

  PriceSpread get_price_spread() const;

  // Get max supply
  uint256_t get_max_supply() const;

  // Get current supply
  uint256_t get_circulating_supply() const;

  // Set current supply
  void set_circulating_supply(const uint256_t& value);

  // Get current supply
  uint256_t get_total_supply() const;

  // Set current supply
  void set_total_supply(const uint256_t& value);

  uint8_t get_is_transferable() const;

  uint8_t get_is_discountable() const;

  Address get_beneficiary() const;

  uint256_t get_buy_price(const uint256_t& token_supply) const;

  uint256_t get_sell_price(const uint256_t& token_supply) const;

private:
  const ContextKey revenue_id{};
  const Curve curve;
  const uint256_t max_supply{};
  uint256_t circulating_supply{};
  uint256_t total_supply{};

  const uint8_t is_transferable{};
  const uint8_t is_discountable{};

  const Address beneficiary{};

  static inline auto log = logger::get("store/contract");
};
