#include "account.h"

Account::Account(const Address& addr)
    : Object(addr)
{
}

Account::Account(const Account& account)
    : Object(account.key)
    , balances(account.balances)
{
}

uint256_t& Account::operator[](const TokenKey& token_key)
{
  return balances[token_key];
}

void Account::debug_create() const
{
  DEBUG(log, "CREATE_ACCOUNT:");
  DEBUG(log, "  Account: {}", key.to_iban_string(IBANType::Account));
  for (auto& [token, value] : balances) {
    DEBUG(log, "  Token: {}, Value: {}",
          token.to_iban_string(IBANType::Contract), value);
  }
}

void Account::debug_save() const
{
  DEBUG(log, "SAVE_ACCOUNT:");
  DEBUG(log, "  Account: {}", key.to_iban_string(IBANType::Account));
  for (auto& [token, value] : balances) {
    DEBUG(log, "  Token: {}, Value: {}",
          token.to_iban_string(IBANType::Contract), value);
  }
}
