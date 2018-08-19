#pragma once

#include <array>
#include <boost/functional/hash.hpp>

template <int SIZE>
class Bytes
{
public:
  Bytes() = default;
  Bytes(const Bytes& bytes) = default;

  static constexpr size_t Size = SIZE;
  static constexpr size_t Bits = SIZE << 3;

  /// Create an n-byte structure from a raw string.
  static Bytes from_raw(const std::string& raw_string);

  /// Parse a hex string into an n-byte structure.
  static Bytes from_hex(const std::string& hex_string);

  /// Parse a boost's uint256 into 64-byte representation.
  static Bytes<32> from_uint256(const uint256_t& int_value);

  /// Convert this structure into a boost's uint256.
  uint256_t as_uint256() const;

  /// Simple comparison operators.
  bool operator==(const Bytes& rhs) const;
  bool operator!=(const Bytes& rhs) const { return !operator==(rhs); }

  ///  Convinient function to check if all bits are zeroes.
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

  /// Return the idx^bit of this structure.
  bool get_bit(size_t idx) const;

  /// Return the idx^th byte of this structure.
  unsigned char get_byte(size_t idx) const { return rawdata[idx]; };

  /// Return the raw representation.
  unsigned char* data() { return rawdata.data(); }

  /// Similar to above, but for const variant.
  const unsigned char* data() const { return rawdata.data(); }

  /// Return a string containing this data.
  std::string to_raw_string() const;

  /// Return a friendly hex representation of this hash value.
  std::string to_string() const;

private:
  std::array<unsigned char, SIZE> rawdata = {};
};

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

////////////////////////////////////////////////////////////////////////////////
static inline unsigned char hex_to_byte(char hex_digit)
{
  if ('0' <= hex_digit && hex_digit <= '9') {
    return hex_digit - '0';
  } else if ('a' <= hex_digit && hex_digit <= 'f') {
    return hex_digit - 'a' + 10;
  } else if ('A' <= hex_digit && hex_digit <= 'F') {
    return hex_digit - 'A' + 10;
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
    ret.rawdata[i] = (hex_to_byte(hex_string[2 * i + 0]) << 4) |
                     (hex_to_byte(hex_string[2 * i + 1]) << 0);
  }
  return ret;
}

template <int SIZE>
Bytes<32> Bytes<SIZE>::from_uint256(const uint256_t& int_value)
{
  Bytes<32> ret;
  export_bits(int_value, ret.rawdata.rbegin(), 8, false);
  return ret;
}

template <int SIZE>
uint256_t Bytes<SIZE>::as_uint256() const
{
  static_assert(SIZE == 32, "Only Bytes<32> can be converted to uint256_t");
  uint256_t ret;
  import_bits(ret, rawdata.begin(), rawdata.end());
  return ret;
  return 0;
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
bool Bytes<SIZE>::get_bit(size_t idx) const
{
  return get_byte(idx >> 3) & (128 >> (idx & 7));
}

template <int SIZE>
std::string Bytes<SIZE>::to_raw_string() const
{
  return std::string((const char*)data(), SIZE);
}

template <int SIZE>
std::string Bytes<SIZE>::to_string() const
{
  std::string ret;
  for (unsigned char b : rawdata) {
    ret += "{:02x}"_format(b);
  }
  return ret;
}
