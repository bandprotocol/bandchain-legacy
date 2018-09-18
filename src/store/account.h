#pragma once

#include <unordered_map>

#include "inc/essential.h"
#include "store/context.h"

class Account final : public ObjectImpl<IBANType::Account>
{
public:
  Account(const Account& account) = default;
  Account(const AccountID& addr);

  uint256_t& operator[](const ContractID& token_key);

  void debug_create() const final;
  void debug_save() const final;

private:
  std::unordered_map<ContractID, uint256_t> balances;

  static inline auto log = logger::get("account");
};
