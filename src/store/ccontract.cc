#include "store/ccontract.h"

#include "crypto/sha256.h"
#include "util/endian.h"

CommunityContract::CommunityContract(Context& _ctx,
                                     const ContractID& _contract_id)
    : ctx(_ctx)
    , contract_id(_contract_id)
    , key(sha256(contract_id))
{
  load();
}

void CommunityContract::save() const
{
  log::debug("Set community contract ID: {} curve: {} current_supply: {}",
             contract_id, curve, supply);
  Buffer buf;
  buf << supply << curve;
  ctx.set(key, buf.to_raw_string());
}

void CommunityContract::load()
{
  auto [raw_data, ok] = ctx.try_get(key);
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
