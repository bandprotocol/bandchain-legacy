#pragma once

#include <array>

template <int SIZE>
class Bytes
{
public:
  Bytes() = default;
  Bytes(const Bytes& hash) = default;

  /// Create an n-byte structure from a raw string.
  static Bytes from_raw(const std::string& raw_string);

  /// Parse a hex string into an n-byte structure.
  static Bytes from_hex(const std::string& hex_string);

  /// Return the size of this structure in bytes.
  size_t size() const { return SIZE; }

  /// Return the raw representation.
  unsigned char* data() { return data_.data(); }

  /// Similar to above, but for const variant.
  const unsigned char* data() const { return data_.data(); }

  /// Return a friendly hex representation of this hash value.
  std::string to_string() const;

private:
  std::array<unsigned char, SIZE> data_;
};
