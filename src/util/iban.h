#include "inc/essential.h"
#include "util/bytes.h"

enum class IBANType {
  Account,
  Token,
};

class IBAN
{
public:
  explicit IBAN(const std::string& iban_string, IBANType iban_type);
  explicit IBAN(const Address& addr, IBANType iban_type);

  /// Return true iff this is an IBAN for an account address.
  bool is_account_iban() const;

  /// Return the addrees representation of this IBAN.
  Address as_addr() const;

  /// Return the string representation of this IBAN.
  std::string to_string() const;

private:
  std::array<char, 4> prefix{};
  std::array<char, 32> account{};
};
