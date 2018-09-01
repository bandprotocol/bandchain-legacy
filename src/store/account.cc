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
  /// If the balance exists in the cache, we can just return that.
  if (auto it = balance_cache.find(token_key); it != balance_cache.end())
    return it->second;

  /// Otherwise, look up the database to find the balance of the addr/token
  /// combo and add that to the cache.
  auto [raw_data, ok] = ctx.try_get(get_context_key(token_key));
  const uint256_t value =
      ok ? Buffer(gsl::make_span(raw_data)).read_all<uint256_t>() : 0;

  /// Update the cache for furthur get_balance calls.
  balance_cache[token_key] = value;
  return value;
}

void Account::set_balance(const TokenKey& token_key, uint256_t value)
{
  /// Update the balance on the persistent data store.
  log::info("Set {} balance of {} to {}", addr, token_key, value);
  ctx.set(get_context_key(token_key), Buffer::serialize(value));

  /// Update the balance cache for furthur get_balance calls.
  balance_cache[token_key] = value;
}

Hash Account::get_context_key(const TokenKey& token_key)
{
  return sha256(addr + token_key);
}
