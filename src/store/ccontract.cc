#include "store/ccontract.h"

#include "crypto/sha256.h"
#include "util/endian.h"

CommunityContract::CommunityContract(Context& _ctx, const TokenKey& _token_key)
    : ctx(_ctx)
    , tokenKey(_token_key)
    , key(sha256(tokenKey))
{
  load();
}

void CommunityContract::save() const
{
  Buffer buf;
  buf << supply << curve;
  ctx.set(key, buf.to_raw_string());
}

void CommunityContract::load()
{
  auto [raw_data, ok] = ctx.try_get(sha256(tokenKey));
  if (ok) {
    Buffer buf(gsl::make_span(raw_data));
    buf >> supply >> curve;
  } else {
    supply = 0;
  }
}

void CommunityContract::set_equation(const Curve& _curve)
{
  Buffer buf;
  buf << _curve;
  buf >> curve;
  save();
}

void CommunityContract::set_current_supply(uint256_t value)
{
  supply = value;
  save();
}
