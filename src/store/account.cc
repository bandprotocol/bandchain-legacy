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
  uint256_t& value = balances[token_key];
  DEBUG(log, "GET_BALANCE:");
  DEBUG(log, "  Account: {}", key.to_iban_string(IBANType::Account));
  DEBUG(log, "  Token: {}", token_key.to_iban_string(IBANType::Contract));
  DEBUG(log, "  Value: {}", value);
  return value;
}
