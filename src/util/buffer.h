#pragma once

#include <vector>

class Buffer
{
public:
  Buffer();

  std::byte* begin();

  std::byte& operator[](size_t idx)
  {
    return buf[idx];
  }

  bool empty();

  size_t size();

  void clear();

  void consume(size_t length);

  void append(std::byte data);

  void append(const std::byte* data, size_t length);

  void append(const Buffer& data);

private:
  std::vector<std::byte> buf;
};
