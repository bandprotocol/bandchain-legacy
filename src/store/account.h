#pragma once

#include "inc/essential.h"
#include "store/context.h"

class Account
{
public:
  /// Create an account view from the blockchain context and account address.
  Account(Context& _ctx, const Address& _addr);

  /// Get the balance of this account for the given token.
  uint256_t get_balance(const TokenKey& token_key);

  /// Set the balance of this account for the given token.
  void set_balance(const TokenKey& token_key, uint256_t value);

  /// Convenient function to get BAND token balance directly.
  uint256_t get_band_balance() { return get_balance({}); }

  /// Convenient function to set BAND token balance directly.
  void set_band_balance(uint256_t value) { return set_balance({}, value); }

private:
  /// Return the key in the database that stores the balance information of
  /// the given token key.
  Hash get_context_key(const TokenKey& token_key);

  Context& ctx;
  const Address& addr;

  std::unordered_map<Address, uint256_t> balance_cache;
};
