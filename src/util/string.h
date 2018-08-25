#pragma once

template <typename T>
inline std::string bytes_to_hex(gsl::span<T> data)
{
  static_assert(sizeof(T) == 1);
  std::string hex;
  for (auto b : data) {
    hex += "{:02x}"_format(static_cast<unsigned char>(b));
  }
  return hex;
}

inline std::string string_to_hex(const std::string& data)
{
  return bytes_to_hex(gsl::make_span(data));
}
