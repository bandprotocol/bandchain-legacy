#include "store/contract.h"

#include "crypto/sha256.h"
#include "util/endian.h"

Contract::Contract(const ContractID& contract_id, const ContextKey& _revenue_id,
                   const Curve& _curve, const uint256_t& _max_supply,
                   uint8_t _is_transferable, uint8_t _is_discountable,
                   const Address& _beneficiary)
    : Object(contract_id)
    , revenue_id(_revenue_id)
    , curve(_curve)
    , max_supply(_max_supply)
    , is_transferable(_is_transferable)
    , is_discountable(_is_discountable)
    , beneficiary(_beneficiary)
{
  DEBUG(log, "CREATE_CONTRACT:");
  DEBUG(log, "  ContractID: {}", key.to_iban_string(IBANType::Contract));
  DEBUG(log, "  RevenueID: {}", revenue_id.to_iban_string(IBANType::Revenue));
  DEBUG(log, "  Beneficiary: {}",
        beneficiary.to_iban_string(IBANType::Account));
  DEBUG(log, "  Curve: {}", curve);
  DEBUG(log, "  SpreadType: {}",
        curve.get_price_spread().get_spread_type()._to_string());
  DEBUG(log, "  SpreadValue: {}", curve.get_price_spread().get_spread_value());
  DEBUG(log, "  MaxSupply: {}", max_supply);
  DEBUG(log, "  Transferable: {}", (bool)is_transferable);
  DEBUG(log, "  Discountable: {}", (bool)is_discountable);
}

ContextKey Contract::get_revenue_id() const { return revenue_id; }

std::string Contract::get_string_equation() const { return curve.to_string(); }

PriceSpread Contract::get_price_spread() const
{
  return curve.get_price_spread();
}

uint256_t Contract::get_max_supply() const { return max_supply; }

uint256_t Contract::get_circulating_supply() const
{
  return circulating_supply;
}

void Contract::set_circulating_supply(const uint256_t& value)
{
  circulating_supply = value;
  DEBUG(log, "SET_CIRCULATING_SUPPLY:");
  DEBUG(log, "  ContractID: {}", key.to_iban_string(IBANType::Contract));
  DEBUG(log, "  CirculatingSupply: {}", circulating_supply);
}

uint256_t Contract::get_total_supply() const { return total_supply; }

void Contract::set_total_supply(const uint256_t& value)
{
  total_supply = value;
  DEBUG(log, "SET_TOTAL_SUPPLY:");
  DEBUG(log, "  ContractID: {}", key.to_iban_string(IBANType::Contract));
  DEBUG(log, "  TotalSupply: {}", total_supply);
}

uint8_t Contract::get_is_transferable() const { return is_transferable; }

uint8_t Contract::get_is_discountable() const { return is_discountable; }

Address Contract::get_beneficiary() const { return beneficiary; }

uint256_t Contract::get_buy_price(const uint256_t& token_supply) const
{
  VarsContext vars(token_supply);
  return curve.apply_buy(vars);
}

uint256_t Contract::get_sell_price(const uint256_t& token_supply) const
{
  VarsContext vars(token_supply);
  return curve.apply_sell(vars);
}
