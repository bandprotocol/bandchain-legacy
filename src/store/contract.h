#pragma once

#include "inc/essential.h"
#include "store/context.h"
#include "store/varcontext.h"
#include "util/equation.h"

class Contract final : public ObjectImpl<IBANType::Contract>
{
public:
  Contract(const Contract& contract) = default;
  Contract(const ContractID& contract_id, const RevenueID& _revenue_id,
           const ContractID& _base_contract_id, const Curve& _buy_curve,
           const Curve& _sell_curve, const uint256_t& _max_supply,
           bool _is_transferable, bool _is_discountable,
           const AccountID& _beneficiary);

  uint256_t get_buy_price(const uint256_t& token_supply) const;

  uint256_t get_sell_price(const uint256_t& token_supply) const;

  void debug_create() const final;
  void debug_save() const final;

public:
  const RevenueID revenue_id;
  const ContractID base_contract_id;
  const Curve buy_curve;
  const Curve sell_curve;
  const uint256_t max_supply;
  const bool is_transferable;
  const bool is_discountable;
  const AccountID beneficiary;

  uint256_t circulating_supply = 0;
  uint256_t total_supply = 0;

  static inline auto log = logger::get("contract");
};
