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
  auto [raw_data, ok] = ctx.try_get(get_context_key(token_key));
  const uint256_t value =
      ok ? Buffer(gsl::make_span(raw_data)).read_all<uint256_t>() : 0;

  return value;
}

void Account::set_balance(const TokenKey& token_key, uint256_t value)
{
  /// Update the balance on the persistent data store.
  log::debug("Set {} balance of {} to {}", addr, token_key, value);
  ctx.set(get_context_key(token_key), Buffer::serialize(value));
}

Hash Account::get_context_key(const TokenKey& token_key)
{
  return sha256(addr + token_key);
}
