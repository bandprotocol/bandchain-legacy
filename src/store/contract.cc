#include "store/contract.h"

#include "crypto/sha256.h"
#include "util/endian.h"

Contract::Contract(const ContractID& contract_id, const ContextKey& _revenue_id,
                   const Curve& _curve, const uint256_t& _max_supply,
                   bool _is_transferable, bool _is_discountable,
                   const Address& _beneficiary)
    : Object(contract_id)
    , revenue_id(_revenue_id)
    , beneficiary(_beneficiary)
    , is_transferable(_is_transferable)
    , is_discountable(_is_discountable)
    , max_supply(_max_supply)
    , curve(_curve)
{
}

Contract::Contract(const Contract& contract)
    : Contract(contract.key, contract.revenue_id, contract.curve,
               contract.max_supply, contract.is_transferable,
               contract.is_discountable, contract.beneficiary)
{
  circulating_supply = contract.circulating_supply;
  total_supply = contract.total_supply;
}

std::string Contract::get_string_equation() const { return curve.to_string(); }

PriceSpread Contract::get_price_spread() const
{
  return curve.get_price_spread();
}

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

void Contract::debug_save() const
{
  DEBUG(log, "SAVE_CONTRACT:");
  DEBUG(log, "  ContractID: {}", key.to_iban_string(IBANType::Contract));
  DEBUG(log, "  RevenueID: {}", revenue_id.to_iban_string(IBANType::Revenue));
  DEBUG(log, "  Beneficiary: {}",
        beneficiary.to_iban_string(IBANType::Account));
  DEBUG(log, "  Curve: {}", curve);
  DEBUG(log, "  SpreadType: {}",
        curve.get_price_spread().get_spread_type()._to_string());
  DEBUG(log, "  SpreadValue: {}", curve.get_price_spread().get_spread_value());
  DEBUG(log, "  MaxSupply: {}", max_supply);
  DEBUG(log, "  Transferable: {}", is_transferable);
  DEBUG(log, "  Discountable: {}", is_discountable);
  DEBUG(log, "  CirculatingSupply: {}", circulating_supply);
  DEBUG(log, "  TotalSupply: {}", total_supply);
}
