#include "account.h"

Account::Account(const AccountID& addr)
    : ObjectImpl(addr)
{
}

uint256_t& Account::operator[](const ContractID& token_key)
{
  return balances[token_key];
}

void Account::debug_create() const
{
  DEBUG(log, "CREATE_ACCOUNT:");
  DEBUG(log, "  Account: {}", key.to_string());
  for (auto& [token, value] : balances) {
    DEBUG(log, "  Token: {}, Value: {}", token.to_string(), value);
  }
}

void Account::debug_save() const
{
  DEBUG(log, "SAVE_ACCOUNT:");
  DEBUG(log, "  Account: {}", key.to_string());
  for (auto& [token, value] : balances) {
    DEBUG(log, "  Token: {}, Value: {}", token.to_string(), value);
  }
}
