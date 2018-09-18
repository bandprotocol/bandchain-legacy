#include "store/contract.h"

#include "crypto/sha256.h"
#include "util/endian.h"

Contract::Contract(const ContractID& contract_id, const ContextKey& _revenue_id,
                   const ContractID& _base_contract_id, const Curve& _buy_curve,
                   const Curve& _sell_curve, const uint256_t& _max_supply,
                   uint8_t _is_transferable, uint8_t _is_discountable,
                   const Address& _beneficiary)
    : Object(contract_id)
    , revenue_id(_revenue_id)
    , base_contract_id(_base_contract_id)
    , buy_curve(_buy_curve)
    , sell_curve(_sell_curve)
    , max_supply(_max_supply)
    , is_transferable(_is_transferable)
    , is_discountable(_is_discountable)
    , beneficiary(_beneficiary)
{
}

Contract::Contract(const Contract& contract)
    : Contract(contract.key, contract.revenue_id, contract.base_contract_id,
               contract.buy_curve, contract.sell_curve, contract.max_supply,
               contract.is_transferable, contract.is_discountable,
               contract.beneficiary)
{
  circulating_supply = contract.circulating_supply;
  total_supply = contract.total_supply;
}

uint256_t Contract::get_buy_price(const uint256_t& token_supply) const
{
  VarsContext vars(token_supply);
  return buy_curve.apply(vars);
}

uint256_t Contract::get_sell_price(const uint256_t& token_supply) const
{
  VarsContext vars(token_supply);
  return sell_curve.apply(vars);
}

void Contract::debug_create() const
{
  DEBUG(log, "CREATE_CONTRACT:");
  DEBUG(log, "  ContractID: {}", key.to_iban_string(IBANType::Contract));
  DEBUG(log, "  RevenueID: {}", revenue_id.to_iban_string(IBANType::Revenue));
  DEBUG(log, "  Beneficiary: {}",
        beneficiary.to_iban_string(IBANType::Account));
  DEBUG(log, "  BuyCurve: {}", buy_curve);
  DEBUG(log, "  SellCurve: {}", sell_curve);
  DEBUG(log, "  MaxSupply: {}", max_supply);
  DEBUG(log, "  Transferable: {}", is_transferable);
  DEBUG(log, "  Discountable: {}", is_discountable);
}

void Contract::debug_save() const
{
  DEBUG(log, "SAVE_CONTRACT:");
  DEBUG(log, "  ContractID: {}", key.to_iban_string(IBANType::Contract));
  DEBUG(log, "  CirculatingSupply: {}", circulating_supply);
  DEBUG(log, "  TotalSupply: {}", total_supply);
}
