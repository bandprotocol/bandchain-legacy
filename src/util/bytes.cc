#include "bytes.h"

namespace
{
unsigned char hex_digit_convert(char hex_digit)
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
} // namespace

template <int SIZE>
Bytes<SIZE> Bytes<SIZE>::from_raw(const std::string& raw_string)
{
  if (raw_string.size() != SIZE) {
    throw std::runtime_error("Invalid hex string length");
  }
  Bytes<SIZE> ret;
  std::copy_n(raw_string.begin(), SIZE, ret.data());
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
    ret.data_[i] = (hex_digit_convert(hex_string[2 * i + 0]) << 4) |
                   (hex_digit_convert(hex_string[2 * i + 1]) << 0);
  }
  return ret;
}

template <int SIZE>
std::string Bytes<SIZE>::to_string() const
{
  std::string ret;
  for (unsigned char c : data_) {
    ret += "{:x}"_format(c);
  }
  return ret;
}

template class Bytes<16>;
template class Bytes<32>;
template class Bytes<64>;
template class Bytes<128>;
template class Bytes<256>;
template class Bytes<512>;
