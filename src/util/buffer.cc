#include "buffer.h"

Buffer::Buffer()
    : buf()
{
  /// Reserve 512 bytes to the buffer to prevent unnecessarily resize.
  buf.reserve(512);
}

Buffer& operator<<(Buffer& buf, uint8_t val)
{
  varint_encode(buf, val);
  return buf;
}

Buffer& operator>>(Buffer& buf, uint8_t& val)
{
  varint_decode(buf, val);
  return buf;
}

Buffer& operator<<(Buffer& buf, uint16_t val)
{
  varint_encode(buf, val);
  return buf;
}

Buffer& operator>>(Buffer& buf, uint16_t& val)
{
  varint_decode(buf, val);
  return buf;
}

Buffer& operator<<(Buffer& buf, uint32_t val)
{
  varint_encode(buf, val);
  return buf;
}

Buffer& operator>>(Buffer& buf, uint32_t& val)
{
  varint_decode(buf, val);
  return buf;
}

Buffer& operator<<(Buffer& buf, uint64_t val)
{
  varint_encode(buf, val);
  return buf;
}

Buffer& operator>>(Buffer& buf, uint64_t& val)
{
  varint_decode(buf, val);
  return buf;
}

Buffer& operator<<(Buffer& buf, const uint256_t& val)
{
  varint_encode(buf, val);
  return buf;
}

Buffer& operator>>(Buffer& buf, uint256_t& val)
{
  varint_decode(buf, val);
  return buf;
}

Buffer& operator<<(Buffer& buf, const bool val)
{
  uint8_t v = val ? 1 : 0;
  return buf << v;
}

Buffer& operator>>(Buffer& buf, bool& val)
{
  uint8_t tmp_val;
  buf >> tmp_val;

  if (tmp_val > 1)
    throw Error("Boolean variable value must be 0 or 1");

  val = (bool)tmp_val;
  return buf;
}

Buffer& operator<<(Buffer& buf, const std::string& val)
{
  return buf << (uint64_t)val.length() << gsl::make_span(val);
}

Buffer& operator>>(Buffer& buf, std::string& val)
{
  val.resize(buf.read<uint64_t>());
  return buf >> gsl::make_span(val);
}
