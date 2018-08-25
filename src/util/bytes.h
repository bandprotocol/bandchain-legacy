#pragma once

#include <array>
#include <boost/functional/hash.hpp>

#include "util/buffer.h"
#include "util/string.h"

template <int SIZE>
class Bytes
{
public:
  Bytes() = default;
  Bytes(const Bytes& bytes) = default;

  /// Convert fixed size integers into bytes.
  explicit Bytes(const uint256_t& int_value);

  static constexpr size_t Size = SIZE;
  static constexpr size_t Bits = SIZE << 3;

  /// Create an n-byte structure from a raw string.
  static Bytes from_raw(const std::string& raw_string);

  /// Parse a hex string into an n-byte structure.
  static Bytes from_hex(const std::string& hex_string);

  /// Convert this structure into a boost's uint256.
  uint256_t as_uint256() const;

  /// Anywhere that accepts span<std::byte> can also accepts Bytes.
  operator gsl::span<std::byte>() { return {data(), SIZE}; }

  /// Similar to above, but for const variant.
  operator gsl::span<const std::byte>() const { return {data(), SIZE}; }

  /// Simple comparison operators.
  bool operator==(const Bytes& rhs) const;
  bool operator!=(const Bytes& rhs) const { return !operator==(rhs); }

  /// Convinient function to check if all bits are zeroes.
  bool is_empty() const { return operator==(Bytes()); }

  /// Concat this bytes with another.
  template <int RHS_SIZE>
  Bytes<SIZE + RHS_SIZE> operator+(const Bytes<RHS_SIZE>& rhs) const;

  /// Return the first RET_SIZE bytes of this structure.
  template <int RET_SIZE>
  Bytes<RET_SIZE> prefix() const;

  /// Return the last RET_SIZE bytes of this structure.
  template <int RET_SIZE>
  Bytes<RET_SIZE> suffix() const;

  /// Return the idx^th byte of this structure.
  std::byte operator[](size_t idx) { return rawdata[idx]; }

  /// Similar to above, but for const variant.
  const std::byte& operator[](size_t idx) const { return rawdata[idx]; }

  /// Return the raw representation.
  std::byte* data() { return rawdata.data(); }

  /// Similar to above, but for const variant.
  const std::byte* data() const { return rawdata.data(); }

  /// Return a string containing this data.
  std::string to_raw_string() const;

  /// Return a friendly hex representation of this hash value.
  std::string to_string() const;

private:
  std::array<std::byte, SIZE> rawdata = {};
} __attribute__((packed));

using Address = Bytes<20>;   //< Public wallet address
using BigInt = Bytes<32>;    //< Big-endian 256 bit unsigned integer
using Hash = Bytes<32>;      //< SHA-256 hash value
using VerifyKey = Bytes<32>; //< Ed25519 verify key
using SecretKey = Bytes<64>; //< Ed25519 secret key
using Signature = Bytes<64>; //< Ed25519 signature

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

template <int SIZE>
inline Buffer& operator<<(Buffer& buf, const Bytes<SIZE>& data)
{
  return buf << data.operator gsl::span<const std::byte>();
}

template <int SIZE>
inline Buffer& operator>>(Buffer& buf, Bytes<SIZE>& data)
{
  return buf >> data.operator gsl::span<std::byte>();
}

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
Bytes<SIZE>::Bytes(const uint256_t& int_value)
{
  static_assert(SIZE == 32, "Only Bytes<32> can be converted from uint256_t");
  export_bits(int_value, (uint8_t*)data(), 8, false);
  std::reverse(rawdata.begin(), rawdata.end());
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
  for (size_t i = 0; i < SIZE; ++i) {
    ret.rawdata[i] = (hex_to_byte(hex_string[2 * i + 0]) << 4) |
                     (hex_to_byte(hex_string[2 * i + 1]) << 0);
  }
  return ret;
}

template <int SIZE>
uint256_t Bytes<SIZE>::as_uint256() const
{
  static_assert(SIZE == 32, "Only Bytes<32> can be converted to uint256_t");
  uint256_t ret;
  import_bits(ret, (uint8_t*)data(), (uint8_t*)data() + SIZE);
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
std::string Bytes<SIZE>::to_raw_string() const
{
  return std::string((const char*)data(), SIZE);
}

template <int SIZE>
std::string Bytes<SIZE>::to_string() const
{
  return bytes_to_hex(this->operator gsl::span<const std::byte>());
}
