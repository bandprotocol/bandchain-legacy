#pragma once

#include <array>
#include <boost/functional/hash.hpp>

template <int SIZE>
class Bytes
{
public:
  Bytes() = default;
  Bytes(const Bytes& bytes) = default;

  /// Create an n-byte structure from a raw string.
  static Bytes from_raw(const std::string& raw_string);

  /// Parse a hex string into an n-byte structure.
  static Bytes from_hex(const std::string& hex_string);

  /// Simple comparison operator.
  bool operator==(const Bytes& rhs) const;

  /// Concat this bytes with another.
  template <int RHS_SIZE>
  Bytes<SIZE + RHS_SIZE> operator+(const Bytes<RHS_SIZE>& rhs) const;

  /// Return the first RET_SIZE bytes of this structure
  template <int RET_SIZE>
  Bytes<RET_SIZE> prefix() const;

  /// Return the last RET_SIZE bytes of this structure
  template <int RET_SIZE>
  Bytes<RET_SIZE> suffix() const;

  /// Return the raw representation.
  std::byte* data() { return rawdata.data(); }

  /// Similar to above, but for const variant.
  const std::byte* data() const { return rawdata.data(); }

  /// Return a friendly hex representation of this hash value.
  std::string to_string() const;

private:
  std::array<std::byte, SIZE> rawdata;
};

using VerifyKey = Bytes<32>;
using SecretKey = Bytes<64>;
using Signature = Bytes<64>;

namespace std
{
template <int SIZE>
struct hash<Bytes<SIZE>> {
  inline size_t operator()(const Bytes<SIZE>& obj) const
  {
    return boost::hash_range(obj.data(), obj.data() + SIZE);
  }
};
} // namespace std

////////////////////////////////////////////////////////////////////////////////
static inline std::byte hex_to_byte(char hex_digit)
{
  if ('0' <= hex_digit && hex_digit <= '9') {
    return std::byte(hex_digit - '0');
  } else if ('a' <= hex_digit && hex_digit <= 'f') {
    return std::byte(hex_digit - 'a' + 10);
  } else if ('A' <= hex_digit && hex_digit <= 'F') {
    return std::byte(hex_digit - 'A' + 10);
  } else {
    throw std::runtime_error("Invalid hex digit character");
  }
}

template <int SIZE>
Bytes<SIZE> Bytes<SIZE>::from_raw(const std::string& raw_string)
{
  if (raw_string.size() != SIZE) {
    throw std::runtime_error("Invalid hex string length");
  }
  Bytes<SIZE> ret;
  std::memcpy(ret.data(), raw_string.c_str(), SIZE);
  return ret;
}

template <int SIZE>
Bytes<SIZE> Bytes<SIZE>::from_hex(const std::string& hex_string)
{
  if (hex_string.size() != 2 * SIZE) {
    throw std::runtime_error("Invalid hex string length");
  }
  Bytes<SIZE> ret;
  for (int i = 0; i < SIZE; ++i) {
    ret.rawdata[i] = std::byte((hex_to_byte(hex_string[2 * i + 0]) << 4) |
                               (hex_to_byte(hex_string[2 * i + 1]) << 0));
  }
  return ret;
}

template <int SIZE>
bool Bytes<SIZE>::operator==(const Bytes<SIZE>& rhs) const
{
  return rawdata == rhs.rawdata;
}

template <int SIZE>
template <int RHS_SIZE>
Bytes<SIZE + RHS_SIZE> Bytes<SIZE>::operator+(const Bytes<RHS_SIZE>& rhs) const
{
  Bytes<SIZE + RHS_SIZE> ret;
  std::memcpy(ret.data(), data(), SIZE);
  std::memcpy(ret.data() + SIZE, rhs.data(), RHS_SIZE);
  return ret;
}

template <int SIZE>
template <int RET_SIZE>
Bytes<RET_SIZE> Bytes<SIZE>::prefix() const
{
  Bytes<RET_SIZE> ret;
  std::memcpy(ret.data(), data(), RET_SIZE);
  return ret;
}

template <int SIZE>
template <int RET_SIZE>
Bytes<RET_SIZE> Bytes<SIZE>::suffix() const
{
  Bytes<RET_SIZE> ret;
  std::memcpy(ret.data(), data() + SIZE - RET_SIZE, RET_SIZE);
  return ret;
}

template <int SIZE>
std::string Bytes<SIZE>::to_string() const
{
  std::string ret;
  for (std::byte b : rawdata) {
    ret += "{:02x}"_format((unsigned char)b);
  }
  return ret;
}
