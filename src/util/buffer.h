#pragma once

#include <vector>

class Buffer
{
public:
  Buffer();

  std::byte* begin();

  bool empty();

  std::size_t size();

  void clear();

  void consume(std::size_t length);

  void append(const std::byte* data, std::size_t length);

  void append(const Buffer& data);

private:
  std::vector<std::byte> buf;
};
