#pragma once

#include <vector>

class Buffer
{
public:
  Buffer();

  std::byte* begin();

  const std::byte& operator[](size_t idx) const
  {
    return buf[idx];
  }

  std::byte& operator[](size_t idx)
  {
    return buf[idx];
  }

  bool empty() const;

  size_t size() const;

  void clear();

  void consume(size_t length);

  void append(std::byte data);

  void append(const std::byte* data, size_t length);

  void append(const Buffer& data);

private:
  std::vector<std::byte> buf;
};
