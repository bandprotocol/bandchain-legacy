#pragma once

#include <enum/enum.h>

#include "inc/essential.h"
#include "util/bytes.h"

BETTER_ENUM(IBANType, char, Account = 'A', Contract = 'C', Price = 'P',
            Revenue = 'R', Stake = 'S')

template <IBANType::_enumerated IBAN_TYPE>
class IBANAddrBase;

class IBANBase
{
protected:
  explicit IBANBase(const std::string& iban_string, IBANType iban_type);
  explicit IBANBase(const Address& addr, IBANType iban_type);

  /// Return the address representation of this IBAN.
  Address as_addr_impl() const;

public:
  /// Return the string representation of this IBAN.
  std::string to_string() const;

private:
  std::array<char, 4> prefix{};
  std::array<char, 32> account{};
};

template <IBANType::_enumerated IBAN_TYPE>
class IBAN : public IBANBase
{
public:
  static constexpr IBANType TYPE = IBAN_TYPE;

  explicit IBAN(const std::string& iban_string)
      : IBANBase(iban_string, TYPE)
  {
  }

  explicit IBAN(const Address& addr)
      : IBANBase(addr, TYPE)
  {
  }

  IBANAddrBase<IBAN_TYPE> as_addr() const;
};

template <IBANType::_enumerated IBAN_TYPE>
class IBANAddrBase : public Address
{
public:
  static constexpr IBANType TYPE = IBAN_TYPE;

  IBANAddrBase() = default;
  IBANAddrBase(const IBANAddrBase&) = default;

  static IBANAddrBase rand() { return IBANAddrBase(Address::rand()); }

  static IBANAddrBase from_addr(const Address& addr)
  {
    return IBANAddrBase(addr);
  }

  static IBANAddrBase from_string(const std::string& str)
  {
    return IBAN<IBAN_TYPE>(str).as_addr();
  }

  std::string to_string() const { return IBAN<IBAN_TYPE>(*this).to_string(); }

private:
  IBANAddrBase(const Address& _addr)
      : Address(_addr)
  {
  }
};

template <IBANType::_enumerated IBAN_TYPE>
IBANAddrBase<IBAN_TYPE> IBAN<IBAN_TYPE>::as_addr() const
{
  return IBANAddrBase<IBAN_TYPE>::from_addr(as_addr_impl());
}

namespace std
{
template <IBANType::_enumerated IBAN_TYPE>
struct hash<IBANAddrBase<IBAN_TYPE>> {
  inline size_t operator()(const IBANAddrBase<IBAN_TYPE>& obj) const
  {
    return boost::hash_range(obj.data(), obj.data() + Address::Size);
  }
};
} // namespace std

using AccountID = IBANAddrBase<IBANType::Account>;
using ContractID = IBANAddrBase<IBANType::Contract>;
using PriceID = IBANAddrBase<IBANType::Price>;
using RevenueID = IBANAddrBase<IBANType::Revenue>;
using StakeID = IBANAddrBase<IBANType::Stake>;
