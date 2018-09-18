#include "revenue.h"

Revenue::Revenue(const ContextKey& revenue_id, const TokenKey& _base_token_id,
                 const Address& _manager, const TimePeriod& _time_period,
                 const ContextKey& _stake_id, bool _is_private)
    : Object(revenue_id)
    , base_token_id(_base_token_id)
    , manager(_manager)
    , time_period(_time_period)
    , stake_id(_stake_id)
    , is_private(_is_private)
{
  DEBUG(log, "CREATE_REVENUE:");
  DEBUG(log, "  RevenueID: {}", key.to_iban_string(IBANType::Revenue));
  DEBUG(log, "  BaseTokenID: {}",
        base_token_id.to_iban_string(IBANType::Contract));
  DEBUG(log, "  Manager: {}", manager.to_iban_string(IBANType::Account));
  // DEBUG(log, "  SpreadType: {}", time_period.);
  // DEBUG(log, "  SpreadValue: {}",
  // curve.get_price_spread().get_spread_value());
  DEBUG(log, "  StakeID: {}", stake_id.to_iban_string(IBANType::Stake));
  DEBUG(log, "  Private: {}", (bool)is_private);
}

Revenue::Revenue(const Revenue& revenue)
    : Revenue(revenue.key, revenue.base_token_id, revenue.manager,
              revenue.time_period, revenue.stake_id, revenue.is_private)
{
}

TokenKey Revenue::get_base_token_id() const { return base_token_id; }

Address Revenue::get_manager() const { return manager; }

ContextKey Revenue::get_stake_id() const { return stake_id; }

bool Revenue::get_is_private() const { return is_private; }

uint64_t Revenue::get_tpc(int64_t timestamp) const
{
  return time_period.timestamp_to_tpc(timestamp);
}

uint256_t Revenue::get_period_revenue(uint64_t period_number) const
{
  auto search = revenues.find(period_number);
  if (search == revenues.end())
    throw Error("Revenue in this period({}) not found.", period_number);

  uint256_t value = search->second;

  DEBUG(log, "GET_REVENUE:");
  DEBUG(log, "  RevenueID: {}", key.to_iban_string(IBANType::Revenue));
  DEBUG(log, "  Period: {}", period_number);
  DEBUG(log, "  Value: {}", value);

  return value;
}

void Revenue::set_period_revenue(uint64_t period_number, const uint256_t& value)
{
  revenues[period_number] = value;
}

uint256_t Revenue::token_to_x(const uint256_t& tokens) const
{
  // TODO
  return tokens;
}

uint256_t Revenue::x_to_token(const uint256_t& x) const
{
  // TODO
  return x;
}

void Revenue::debug_create() const
{
  DEBUG(log, "CREATE_REVENUE:");
  DEBUG(log, "  RevenueID: {}", key.to_iban_string(IBANType::Revenue));
  DEBUG(log, "  BaseTokenID: {}",
        base_token_id.to_iban_string(IBANType::Contract));
  DEBUG(log, "  Manager: {}", manager.to_iban_string(IBANType::Account));
  DEBUG(log, "  TimeUnit: {}", time_period.time_unit._to_string());
  DEBUG(log, "  TimeValue: {}", time_period.time_value);
  DEBUG(log, "  StakeID: {}", stake_id.to_iban_string(IBANType::Stake));
  DEBUG(log, "  Private: {}", is_private);
}

void Revenue::debug_save() const
{
  DEBUG(log, "SAVE_REVENUE:");
  DEBUG(log, "  RevenueID: {}", key.to_iban_string(IBANType::Revenue));
  DEBUG(log, "  BaseTokenID: {}",
        base_token_id.to_iban_string(IBANType::Contract));
}
