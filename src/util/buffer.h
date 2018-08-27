#pragma once

#include <vector>

#include "util/string.h"

class Buffer
{
public:
  operator gsl::span<std::byte>() { return gsl::make_span(buf); }
  operator gsl::span<const std::byte>() const { return gsl::make_span(buf); }

  std::byte* begin() { return &(*buf.begin()); }

  std::byte* reserve(size_t reserve_size)
  {
    size_t current_size = size_bytes();
    buf.resize(current_size + reserve_size);
    return &buf[current_size];
  }

  const std::byte& operator[](size_t idx) const { return buf[idx]; }

  std::byte& operator[](size_t idx) { return buf[idx]; }

  bool empty() const { return buf.empty(); }

  size_t size_bytes() const { return buf.size(); }

  void clear() { buf.clear(); }

  void consume(size_t length) { buf.erase(buf.begin(), buf.begin() + length); }

  void append(std::byte data) { buf.push_back(data); }

  void append(const Buffer& data)
  {
    buf.insert(buf.end(), data.buf.begin(), data.buf.end());
  }

  std::string to_string() const
  {
    return bytes_to_hex(operator gsl::span<const std::byte>());
  }

private:
  std::vector<std::byte> buf;
};

template <typename T>
inline Buffer& operator<<(Buffer& buf, gsl::span<T> data)
{
  std::memcpy(buf.reserve(data.size_bytes()), data.data(), data.size_bytes());
  return buf;
}

template <typename T>
inline Buffer& operator>>(Buffer& buf, gsl::span<T> data)
{
  if (buf.size_bytes() < data.size_bytes())
    throw Error("Buffer is too short. {} < {}.", buf.size_bytes(),
                data.size_bytes());
  std::memcpy(data.data(), buf.begin(), data.size_bytes());
  buf.consume(data.size_bytes());
  return buf;
}
