#pragma once

#include <boost/endian/arithmetic.hpp>

using boost::endian::big_uint8_t;

using boost::endian::big_uint16_t;
using boost::endian::big_uint32_t;
using boost::endian::big_uint64_t;

template <typename BASE>
class big_uint
{
public:
  big_uint() = default;
  big_uint(const big_uint& rhs) = default;
  big_uint(big_uint&& rhs) = default;

  template <typename INT, typename = std::enable_if_t<std::is_integral_v<INT>>>
  big_uint(INT rhs);

  big_uint& operator=(const big_uint& rhs) = default;
  big_uint& operator=(big_uint&& rhs) = default;

  big_uint& operator++() { return *this += 1; }
  big_uint& operator--() { return *this -= 1; }
  big_uint operator++(int);
  big_uint operator--(int);

  big_uint& operator+=(const big_uint& rhs);
  big_uint& operator-=(const big_uint& rhs);
  big_uint& operator*=(const big_uint& rhs);
  big_uint& operator/=(const big_uint& rhs);
  big_uint& operator%=(const big_uint& rhs);

  big_uint operator+(const big_uint& rhs) const;
  big_uint operator-(const big_uint& rhs) const;
  big_uint operator*(const big_uint& rhs) const;
  big_uint operator/(const big_uint& rhs) const;
  big_uint operator%(const big_uint& rhs) const;

  bool operator==(const big_uint& rhs) const;
  bool operator<(const big_uint& rhs) const;
  bool operator>(const big_uint& rhs) const;
  bool operator!=(const big_uint& rhs) const { return !operator==(rhs); }
  bool operator<=(const big_uint& rhs) const { return !operator>(rhs); }
  bool operator>=(const big_uint& rhs) const { return !operator<(rhs); }

  int hello() const { return 42; }

private:
  BASE hi;
  BASE lo;
};

using big_uint128_t = big_uint<big_uint64_t>;
using big_uint256_t = big_uint<big_uint128_t>;

////////////////////////////////////////////////////////////////////////////////
template <typename BASE>
template <typename INT, typename>
big_uint<BASE>::big_uint(INT rhs)
    : hi()
    , lo(rhs)
{
}

template <typename BASE>
big_uint<BASE> big_uint<BASE>::operator++(int)
{
  auto copy = *this;
  ++*this;
  return copy;
}

template <typename BASE>
big_uint<BASE> big_uint<BASE>::operator--(int)
{
  auto copy = *this;
  --*this;
  return copy;
}

template <typename BASE>
bool big_uint<BASE>::operator==(const big_uint<BASE>& rhs) const
{
  return std::make_pair(hi, lo) == std::make_pair(rhs.hi, rhs.lo);
}

template <typename BASE>
bool big_uint<BASE>::operator<(const big_uint<BASE>& rhs) const
{
  return std::make_pair(hi, lo) < std::make_pair(rhs.hi, rhs.lo);
}

template <typename BASE>
bool big_uint<BASE>::operator>(const big_uint<BASE>& rhs) const
{
  return std::make_pair(hi, lo) > std::make_pair(rhs.hi, rhs.lo);
}

template <typename BASE>
big_uint<BASE>& big_uint<BASE>::operator+=(const big_uint<BASE>& rhs)
{
  hi += rhs.hi;
  hi += ((lo + rhs.lo) < lo);
  lo += rhs.lo;
}

template <typename BASE>
big_uint<BASE>& big_uint<BASE>::operator-=(const big_uint<BASE>& rhs)
{
  hi -= rhs.hi;
  hi -= ((lo - rhs.lo) > lo);
  lo -= rhs.lo;
}

template <typename BASE>
big_uint<BASE>& big_uint<BASE>::operator*=(const big_uint<BASE>& rhs)
{
  hi -= rhs.hi;
  hi -= ((lo - rhs.lo) > lo);
  lo -= rhs.lo;
}
