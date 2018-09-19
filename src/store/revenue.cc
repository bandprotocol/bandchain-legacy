#include "revenue.h"

#include "store/varcontext.h"
Revenue::Revenue(const RevenueID& revenue_id, const ContractID& _base_token_id,
                 const AccountID& _manager, const TimePeriod& _time_period,
                 const StakeID& _stake_id, bool _is_private,
                 const Curve& _curve_to_base, const Curve& _curve_to_x)
    : ObjectImpl(revenue_id)
    , base_token_id(_base_token_id)
    , manager(_manager)
    , time_period(_time_period)
    , stake_id(_stake_id)
    , is_private(_is_private)
    , curve_to_base(_curve_to_base)
    , curve_to_x(_curve_to_x)
{
}

uint64_t Revenue::get_tpc(int64_t timestamp) const
{
  return time_period.timestamp_to_tpc(timestamp);
}

uint256_t& Revenue::operator[](uint64_t tpc) { return revenues[tpc]; }

uint256_t Revenue::base_to_x(const uint256_t& base) const
{

  VarsContext vars(base);
  return curve_to_x.apply(vars);
}

uint256_t Revenue::x_to_base(const uint256_t& x) const
{
  // TODO
  VarsContext vars(x);
  return curve_to_base.apply(vars);
}

void Revenue::debug_create() const
{
  DEBUG(log, "CREATE_REVENUE:");
  // DEBUG(log, "  RevenueID: {}", key.to_iban_string(IBANType::Revenue));
  // DEBUG(log, "  BaseTokenID: {}",
  //       base_token_id.to_iban_string(IBANType::Contract));
  // DEBUG(log, "  Manager: {}", manager.to_iban_string(IBANType::Account));
  DEBUG(log, "  TimeUnit: {}", time_period.time_unit._to_string());
  DEBUG(log, "  TimeValue: {}", time_period.time_value);
  // DEBUG(log, "  StakeID: {}", stake_id.to_iban_string(IBANType::Stake));
  DEBUG(log, "  Private: {}", is_private);
}

void Revenue::debug_save() const
{
  DEBUG(log, "SAVE_REVENUE:");
  // DEBUG(log, "  RevenueID: {}", key.to_iban_string(IBANType::Revenue));
  // DEBUG(log, "  BaseTokenID: {}",
  //       base_token_id.to_iban_string(IBANType::Contract));
}
