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
  Curve curve;
  buf << _curve;
  buf >> info.curve;
  info.max_supply = 0;
  info.current_supply = 0;
  save(std::move(info));
}

void CommunityContract::save(const ContractInfo& info) const
{
  log::debug("Set community contract ID: {} curve: {} current_supply: {} "
             "max_supply: {}",
             contract_id, info.curve, info.current_supply, info.max_supply);
  Buffer buf;
  buf << info.curve << info.current_supply << info.max_supply;
  ctx.set(key, buf.to_raw_string());
}

CommunityContract::ContractInfo CommunityContract::load() const
{
  ContractInfo info;
  auto [raw_data, ok] = ctx.try_get(key);
  if (ok) {
    Buffer buf(gsl::make_span(raw_data));
    buf >> info.curve >> info.current_supply >> info.max_supply;
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

void CommunityContract::set_current_supply(uint256_t value)
{
  ContractInfo info = load();
  info.current_supply = value;
  save(std::move(info));
}

uint256_t CommunityContract::get_current_supply() const
{
  return load().current_supply;
}

uint256_t CommunityContract::apply_equation(uint256_t x, bool is_sell) const
{
  Curve curve = load().curve;
  VarsContext vars(ctx, x);
  return curve.apply(vars, is_sell);
}

void CommunityContract::set_max_supply(uint256_t value)
{
  ContractInfo info = load();
  info.max_supply = value;
  save(std::move(info));
}

uint256_t CommunityContract::get_max_supply() const
{
  return load().max_supply;
}
