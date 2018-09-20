#pragma once

#include <array>
#include <boost/functional/hash.hpp>

#include "crypto/random.h"
#include "inc/essential.h"
#include "util/buffer.h"
#include "util/string.h"

template <int SIZE>
class Bytes
{
public:
  Bytes() = default;
  Bytes(const Bytes& bytes) = default;

  static constexpr size_t Size = SIZE;

  /// Create an n-byte structure from a raw string.
  static Bytes from_raw(const std::string& raw_string);

  /// Parse a hex string into an n-byte structure.
  static Bytes from_hex(const std::string& hex_string);

  /// Create an arbitrary n-byte structure. Useful for testing.
  static Bytes rand();

  /// Expose this data as a read-only span. Note that if this is destroyed,
  /// the span will become invalid.
  gsl::span<const std::byte> as_span() const { return gsl::make_span(rawdata); }

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
  std::byte& operator[](size_t idx) { return rawdata[idx]; }

  /// Similar to above, but for const variant.
  const std::byte& operator[](size_t idx) const { return rawdata[idx]; }

  /// Return the raw representation.
  std::byte* data() { return rawdata.data(); }

  /// Similar to above, but for const variant.
  const std::byte* data() const { return rawdata.data(); }

  /// Return a friendly hex representation of this bytes value.
  std::string to_string() const;

  /// Read and write from/to buffer.
  friend Buffer& operator<<(Buffer& buf, const Bytes& data)
  {
    return buf << gsl::make_span(data.rawdata);
  }
  friend Buffer& operator>>(Buffer& buf, Bytes& data)
  {
    return buf >> gsl::make_span(data.rawdata);
  }

private:
  std::array<std::byte, SIZE> rawdata{};
} __attribute__((packed));

using Address = Bytes<20>;   //< 20-Byte address
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
static inline std::byte hex_to_byte(char hex_digit)
{
  if ('0' <= hex_digit && hex_digit <= '9') {
    return std::byte(hex_digit - '0');
  } else if ('a' <= hex_digit && hex_digit <= 'f') {
    return std::byte(hex_digit - 'a' + 10);
  } else if ('A' <= hex_digit && hex_digit <= 'F') {
    return std::byte(hex_digit - 'A' + 10);
  } else {
    throw Error("Invalid hex digit character");
  }
}

template <int SIZE>
Bytes<SIZE> Bytes<SIZE>::from_raw(const std::string& raw_string)
{
  if (raw_string.size() != SIZE) {
    throw Error("Invalid hex string length");
  }
  Bytes<SIZE> ret;
  std::memcpy(ret.data(), raw_string.c_str(), SIZE);
  return ret;
}

template <int SIZE>
Bytes<SIZE> Bytes<SIZE>::from_hex(const std::string& hex_string)
{
  if (hex_string.size() != 2 * SIZE) {
    throw Error("Invalid hex string length");
  }
  Bytes<SIZE> ret;
  for (size_t i = 0; i < SIZE; ++i) {
    ret.rawdata[i] = (hex_to_byte(hex_string[2 * i + 0]) << 4) |
                     (hex_to_byte(hex_string[2 * i + 1]) << 0);
  }
  return ret;
}

template <int SIZE>
Bytes<SIZE> Bytes<SIZE>::rand()
{
  Bytes<SIZE> ret;
  random_bytes(gsl::make_span(ret.rawdata));
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
  return bytes_to_hex(as_span());
}
