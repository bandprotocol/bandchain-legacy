#include "store/ccontract.h"

#include "crypto/sha256.h"
#include "util/endian.h"

CommunityContract::CommunityContract(Context& _ctx,
                                     const ContractID& _contract_id)
    : ctx(_ctx)
    , contract_id(_contract_id)
    , key(sha256(contract_id))
{
}

void CommunityContract::create(const Curve& _curve)
{
  if (ctx.check(key))
    throw Error("This key already have contract cannot create new contract at "
                "this key");
  // Create empty contract to context at this key.
  ContractInfo info;
  Buffer buf;
  buf << _curve;
  buf >> info.curve;
  info.max_supply = 0;
  info.current_supply = 0;
  info.current_profit = 0;
  save(std::move(info));
}

void CommunityContract::save(const ContractInfo& info) const
{
  log::debug("Set community contract ID: {} curve: {} max_supply: "
             "{} current_supply: {}"
             " current_profit: {}",
             contract_id, info.curve, info.max_supply, info.current_supply,
             info.current_profit);
  Buffer buf;
  buf << info.curve << info.max_supply << info.current_supply
      << info.current_profit;
  ctx.set(key, buf.to_raw_string());
}

CommunityContract::ContractInfo CommunityContract::load() const
{
  ContractInfo info;
  auto [raw_data, ok] = ctx.try_get(key);
  if (ok) {
    Buffer buf(gsl::make_span(raw_data));
    buf >> info.curve >> info.max_supply >> info.current_supply >>
        info.current_profit;
    return info;
  } else {
    throw Error("Not found any contract to match this key.");
  }
}

std::string CommunityContract::get_string_equation() const
{
  Curve curve = load().curve;
  return curve.to_string();
}

PriceSpread CommunityContract::get_price_spread() const
{
  return load().curve.get_price_spread();
}

uint256_t CommunityContract::get_current_supply() const
{
  return load().current_supply;
}

void CommunityContract::set_current_supply(uint256_t value)
{
  ContractInfo info = load();
  info.current_supply = value;
  save(std::move(info));
}

uint256_t CommunityContract::get_max_supply() const
{
  return load().max_supply;
}

void CommunityContract::set_max_supply(uint256_t value)
{
  ContractInfo info = load();
  info.max_supply = value;
  save(std::move(info));
}

uint256_t CommunityContract::get_current_profit() const
{
  return load().current_profit;
}

void CommunityContract::set_current_profit(uint256_t value)
{
  ContractInfo info = load();
  info.current_profit = value;
  save(std::move(info));
}

uint256_t CommunityContract::get_buy_price(uint256_t token_supply) const
{
  Curve curve = load().curve;
  VarsContext vars(ctx, token_supply);
  return curve.apply_buy(vars);
}

uint256_t CommunityContract::get_sell_price(uint256_t token_supply) const
{
  Curve curve = load().curve;
  VarsContext vars(ctx, token_supply);
  return curve.apply_sell(vars);
}
