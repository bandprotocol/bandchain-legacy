#pragma once

#include <enum/enum.h>

#include "inc/essential.h"

template <int SIZE>
class Bytes;

BETTER_ENUM(IBANType, char, Account = 'A', Token = 'C')

class IBAN
{
public:
  explicit IBAN(const std::string& iban_string, IBANType iban_type);
  explicit IBAN(const Bytes<20>& addr, IBANType iban_type);

  /// Return true iff this is an IBAN for an account address.
  bool is_account_iban() const;

  /// Return the address representation of this IBAN.
  Bytes<20> as_addr() const;

  /// Return the string representation of this IBAN.
  std::string to_string() const;

private:
  std::array<char, 4> prefix{};
  std::array<char, 32> account{};
};
