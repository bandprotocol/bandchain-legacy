#include "account.h"

#include "crypto/sha256.h"
#include "util/bytes.h"
#include "util/endian.h"

Account::Account(Context& _ctx, const Address& _addr)
    : ctx(_ctx)
    , addr(_addr)
{
}

uint256_t Account::get_balance(const TokenKey& token_key)
{
  // TODO
  // auto [raw_data, ok] = ctx.try_get(get_context_key(token_key));
  std::string raw_data = "TODO";
  bool ok = false;

  const uint256_t value =
      ok ? Buffer(gsl::make_span(raw_data)).read_all<uint256_t>() : 0;

  DEBUG(log, "GET_BALANCE:");
  DEBUG(log, "  Account: {}", addr.to_iban_string(IBANType::Account));
  DEBUG(log, "  Token: {}", token_key.to_iban_string(IBANType::Token));
  DEBUG(log, "  Value: {}", value);

  return value;
}

void Account::set_balance(const TokenKey& token_key, uint256_t value)
{
  DEBUG(log, "SET_BALANCE:");
  DEBUG(log, "  Account: {}", addr.to_iban_string(IBANType::Account));
  DEBUG(log, "  Token: {}", token_key.to_iban_string(IBANType::Token));
  DEBUG(log, "  Value: {}", value);

  // TODO
  // ctx.set(get_context_key(token_key), Buffer::serialize(value));
}

Hash Account::get_context_key(const TokenKey& token_key)
{
  return sha256(addr + token_key);
}
