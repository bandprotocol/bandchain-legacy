#pragma once

#include <vector>

#include "util/string.h"

class Buffer
{
public:
  Buffer();

  std::byte* begin();

  std::byte* reserve(size_t reserve_size)
  {
    size_t current_size = size_bytes();
    buf.resize(current_size + reserve_size);
    return &buf[current_size];
  }

  const std::byte& operator[](size_t idx) const { return buf[idx]; }

  std::byte& operator[](size_t idx) { return buf[idx]; }

  bool empty() const;

  size_t size_bytes() const;

  void clear();

  void consume(size_t length);

  void append(std::byte data);

  void append(const Buffer& data);

  std::string to_string() const { return bytes_to_hex(gsl::make_span(buf)); }

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
    throw Error("Out of data - %zu < %zu", buf.size_bytes(), data.size_bytes());
  std::memcpy(data.data(), buf.begin(), data.size_bytes());
  buf.consume(data.size_bytes());
  return buf;
}
