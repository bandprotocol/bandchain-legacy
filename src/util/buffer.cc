#include "buffer.h"

Buffer::Buffer() {}

std::byte* Buffer::begin() { return &(*buf.begin()); }

bool Buffer::empty() const { return buf.empty(); }

size_t Buffer::size_bytes() const { return buf.size(); }

void Buffer::clear() { buf.clear(); }

void Buffer::consume(size_t length)
{
  buf.erase(buf.begin(), buf.begin() + length);
}

void Buffer::append(std::byte data) { buf.push_back(data); }

void Buffer::append(const std::byte* data, size_t length)
{
  buf.insert(buf.end(), data, data + length);
}

void Buffer::append(const Buffer& data)
{
  buf.insert(buf.end(), data.buf.begin(), data.buf.end());
}
