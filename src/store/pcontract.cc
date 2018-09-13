#include "store/pcontract.h"

#include "crypto/sha256.h"
#include "util/endian.h"

ProductContract::ProductContract(Context& _ctx, const ContractID& _contract_id)
    : ctx(_ctx)
    , contract_id(_contract_id)
    , key(sha256(contract_id))
{
}

void ProductContract::create(const Curve& _curve,
                             const ContractID& _contract_id)
{
  // TODO
  // if (ctx.check(key))
  if (true)
    throw Error("This key already have contract cannot create new contract at "
                "this key");
  // Create empty contract to context at this key.
  ContractInfo info;
  Buffer buf;
  buf << _curve;
  buf >> info.curve;
  info.max_supply = 0;
  info.current_supply = 0;
  info.community_contract = _contract_id;
  save(std::move(info));
}

void ProductContract::save(const ContractInfo& info) const
{
  DEBUG(log, "SAVE_CONTRACT:");
  DEBUG(log, "  ContractID: {}", contract_id.to_iban_string(IBANType::Token));
  DEBUG(log, "  Curve: {}", info.curve);
  DEBUG(log, "  MaxSupply: {}", info.max_supply);
  DEBUG(log, "  CurrentSupply: {}", info.current_supply);
  DEBUG(log, "  CommunityContractID: {}", info.community_contract);

  Buffer buf;
  buf << info.curve << info.max_supply << info.current_supply
      << info.community_contract;

  // TODO
  // ctx.set(key, buf.to_raw_string());
}

ProductContract::ContractInfo ProductContract::load() const
{
  ContractInfo info;
  // TODO
  // auto [raw_data, ok] = ctx.try_get(key);
  std::string raw_data = "TODO";
  bool ok = false;

  if (ok) {
    Buffer buf(gsl::make_span(raw_data));
    buf >> info.curve >> info.max_supply >> info.current_supply >>
        info.community_contract;
    return info;
  } else {
    throw Error("Not found any contract to match this key.");
  }
}

std::string ProductContract::get_string_equation() const
{
  Curve curve = load().curve;
  return curve.to_string();
}

PriceSpread ProductContract::get_price_spread() const
{
  return load().curve.get_price_spread();
}

uint256_t ProductContract::get_current_supply() const
{
  return load().current_supply;
}

void ProductContract::set_current_supply(uint256_t value)
{
  ContractInfo info = load();
  info.current_supply = value;
  save(std::move(info));
}

uint256_t ProductContract::get_max_supply() const { return load().max_supply; }

void ProductContract::set_max_supply(uint256_t value)
{
  ContractInfo info = load();
  info.max_supply = value;
  save(std::move(info));
}

ContractID ProductContract::get_community_contract()
{
  return load().community_contract;
}

uint256_t ProductContract::get_buy_price(uint256_t token_supply,
                                         uint256_t ct_price) const
{
  Curve curve = load().curve;
  VarsContextPT vars(ctx, token_supply, ct_price);
  return curve.apply_buy(vars);
}

uint256_t ProductContract::get_sell_price(uint256_t token_supply,
                                          uint256_t ct_price) const
{
  Curve curve = load().curve;
  VarsContextPT vars(ctx, token_supply, ct_price);
  return curve.apply_sell(vars);
}
