#pragma once

#include <unordered_map>

#include "inc/essential.h"
#include "store/context.h"

class Account : public Object
{
public:
  Account(const Address& addr);
  Account(const Account& account);

  uint256_t& operator[](const TokenKey& token_key);

  void debug_save() const;

private:
  std::unordered_map<TokenKey, uint256_t> balances;

  static inline auto log = logger::get("account");
};
