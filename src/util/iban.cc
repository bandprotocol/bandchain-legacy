#include "iban.h"

#include "util/bytes.h"

namespace
{
std::string char_to_int_str(char c)
{
  if ('c' >= '0' && c <= '9')
    return std::string(1, c);
  if ('c' >= 'A' && c <= 'Z')
    return std::to_string(c - 'A' + 10);

  throw Error("Cannot convert ascii {} to int string", (int)c);
}

int compute_remainder(const std::array<char, 32>& account,
                      const std::array<char, 4>& prefix)
{
  std::string ret;

  for (char c : account)
    ret += char_to_int_str(c);
  for (char c : prefix)
    ret += char_to_int_str(c);

  return int(uint256_t(ret) % 97);
}

char base32_encode(int v)
{
  if (v < 8)
    return 'A' + v;
  if (v < 13)
    return 'J' + v - 8;
  if (v < 24)
    return 'P' + v - 13;
  if (v < 32)
    return '2' + v - 24;

  throw Error("Cannot base32_encode value {}", v);
}

int base32_decode(char c)
{
  if (c >= 'A' && c < 'I')
    return c - 'A';
  if (c > 'I' && c < 'O')
    return c - 'A' - 1;
  if (c > 'O' && c <= 'Z')
    return c - 'A' - 2;
  if (c >= '2' && c <= '9')
    return c - '2' + 24;

  throw Error("Cannot base32_decode ascii {}", (int)c);
}
} // namespace

IBAN::IBAN(const std::string& iban_string, IBANType iban_type)
{
  int digit = 0;

  for (char c : iban_string) {
    if (c == ' ')
      continue;

    if (digit >= 36)
      throw Error("IBAN string must contain <= 36 characters");

    if (digit < 4)
      prefix[digit] = c;
    else
      account[digit - 4] = c;

    ++digit;
  }

  if (digit != 36)
    throw Error("IBAN string must contain 36 characters");

  if (prefix[1] != 'X')
    throw Error("IBAN has invalid country code");

  if (iban_type == IBANType::Account && prefix[0] != 'A')
    throw Error("Account IBAN must have country code AX");

  if (iban_type == IBANType::Token && prefix[0] != 'B')
    throw Error("Token IBAN must have country code BX");

  if (compute_remainder(account, prefix) != 1)
    throw Error("Invalid IBAN checksum");
}

IBAN::IBAN(const Address& addr, IBANType iban_type)
{
  prefix[0] = iban_type == IBANType::Account ? 'A' : 'B';
  prefix[1] = 'X';

  uint256_t addr_value;
  for (auto b : addr.as_span()) {
    addr_value = addr_value * 256 + uint8_t(b);
  }

  for (int digit = 31; digit >= 0; --digit) {
    account[digit] = base32_encode(int(addr_value % 32));
    addr_value /= 32;
  }

  prefix[2] = '0';
  prefix[3] = '0';
  int checksum = 98 - compute_remainder(account, prefix);
  prefix[2] = '0' + (checksum / 10);
  prefix[3] = '0' + (checksum % 10);
}

bool IBAN::is_account_iban() const { return prefix[0] == 'A'; }

Address IBAN::as_addr() const
{
  Address ret;

  uint256_t addr_value;
  for (char c : account) {
    addr_value = addr_value * 32 + base32_decode(c);
  }

  for (int digit = Address::Size - 1; digit >= 0; --digit) {
    ret[digit] = std::byte(uint8_t(addr_value & 255));
    addr_value >>= 8;
  }

  return ret;
}

std::string IBAN::to_string() const
{
  std::string ret;
  ret.append(prefix.begin(), prefix.end());
  for (int idx = 0; idx < 8; ++idx) {
    ret.append(" ");
    ret.append(account.begin() + 4 * idx, account.begin() + 4 * idx + 4);
  }
  return ret;
}
