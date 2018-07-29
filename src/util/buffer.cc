#include "buffer.h"

Buffer::Buffer()
{
}

std::byte* Buffer::begin()
{
  return &(*buf.begin());
}

bool Buffer::empty()
{
  return buf.empty();
}

std::size_t Buffer::size()
{
  return buf.size();
}

void Buffer::clear()
{
  buf.clear();
}

void Buffer::consume(std::size_t length)
{
  buf.erase(buf.begin(), buf.begin() + length);
}

void Buffer::append(const std::byte* data, std::size_t length)
{
  buf.insert(buf.end(), data, data + length);
}

void Buffer::append(const Buffer& data)
{
  buf.insert(buf.end(), data.buf.begin(), data.buf.end());
}
