#pragma once

#include <unordered_map>

#include "inc/essential.h"
#include "store/context.h"

class Account : public Object
{
public:
  /// Create an account view from the blockchain context and account address.
  Account(const Address& addr);

  /// Get the balance of this account for the given token.
  uint256_t get_balance(const TokenKey& token_key);

  /// Set the balance of this account for the given token.
  void set_balance(const TokenKey& token_key, const uint256_t& value);

  /// Convenient function to get BAND token balance directly.
  uint256_t get_band_balance() { return get_balance({}); }

  /// Convenient function to set BAND token balance directly.
  void set_band_balance(const uint256_t& value)
  {
    return set_balance({}, value);
  }

private:
  std::unordered_map<TokenKey, uint256_t> balances;
  Address addr;

  static inline auto log = logger::get("store/account");
};
