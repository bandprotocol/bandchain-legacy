#pragma once

#include <boost/endian/buffers.hpp>
#include <sstream>
#include <string>

inline std::string bytes_to_hex(gsl::span<const std::byte> data)
{
  std::string hex;
  for (auto b : data) {
    hex += "{:02x}"_format(static_cast<unsigned char>(b));
  }
  return hex;
}

inline std::string string_to_hex(const std::string& data)
{
  std::string hex;
  for (auto b : data) {
    hex += "{:02x}"_format(static_cast<unsigned char>(b));
  }
  return hex;
}
