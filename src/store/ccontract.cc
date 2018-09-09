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

void CommunityContract::save(std::pair<uint256_t, Curve> data) const
{
  log::debug("Set community contract ID: {} curve: {} current_supply: {}",
             contract_id, data.second, data.first);
  Buffer buf;
  buf << data.first << data.second;
  ctx.set(key, buf.to_raw_string());
}

std::pair<uint256_t, Curve> CommunityContract::load() const
{
  auto [raw_data, ok] = ctx.try_get(key);
  if (ok) {
    Buffer buf(gsl::make_span(raw_data));
    std::pair<uint256_t, Curve> data;
    buf >> data.first >> data.second;
    return data;
  } else {
    return {0, Curve()};
  }
}

std::string CommunityContract::get_string_equation() const
{
  Curve curve = load().second;
  return curve.to_string();
}

void CommunityContract::set_equation(const Curve& _curve)
{
  std::pair<uint256_t, Curve> data = load();
  Buffer buf;
  Curve curve;
  buf << _curve;
  buf >> data.second;
  save(std::move(data));
}

void CommunityContract::set_current_supply(uint256_t value)
{
  auto data = load();
  data.first = value;
  save(std::move(data));
}

uint256_t CommunityContract::get_current_supply() const { return load().first; }

uint256_t CommunityContract::apply_equation(uint256_t x) const
{
  Curve curve = load().second;
  VarsContext vars(ctx, x);
  return curve.apply(vars);
}
