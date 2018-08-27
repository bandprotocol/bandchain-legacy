#pragma once

#include <boost/endian/buffers.hpp>
#include <type_traits>

#include "util/buffer.h"

template <typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
inline Buffer& operator<<(Buffer& buf, T val)
{
  return buf << static_cast<std::underlying_type_t<T>>(val);
}

template <typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
inline Buffer& operator>>(Buffer& buf, T& val)
{
  std::underlying_type_t<T> _val;
  buf >> _val;
  val = T(_val);
  return buf;
}

inline Buffer& operator<<(Buffer& buf, uint8_t val)
{
  const boost::endian::big_uint8_buf_t big_endian_val(val);
  return buf << gsl::span(&big_endian_val, 1);
}

inline Buffer& operator<<(Buffer& buf, uint16_t val)
{
  const boost::endian::big_uint16_buf_t big_endian_val(val);
  return buf << gsl::span(&big_endian_val, 1);
}

inline Buffer& operator<<(Buffer& buf, uint32_t val)
{
  const boost::endian::big_uint32_buf_t big_endian_val(val);
  return buf << gsl::span(&big_endian_val, 1);
}

inline Buffer& operator<<(Buffer& buf, uint64_t val)
{
  const boost::endian::big_uint64_buf_t big_endian_val(val);
  return buf << gsl::span(&big_endian_val, 1);
}

inline Buffer& operator<<(Buffer& buf, const uint256_t& val)
{
  std::array<uint8_t, 32> temp_buf{};
  export_bits(val, temp_buf.rbegin(), 8, false);
  return buf << gsl::make_span(temp_buf);
}

inline Buffer& operator>>(Buffer& buf, uint8_t& val)
{
  boost::endian::big_uint8_buf_t big_endian_val;
  buf >> gsl::span(&big_endian_val, 1);
  val = big_endian_val.value();
  return buf;
}

inline Buffer& operator>>(Buffer& buf, uint16_t& val)
{
  boost::endian::big_uint16_buf_t big_endian_val;
  buf >> gsl::span(&big_endian_val, 1);
  val = big_endian_val.value();
  return buf;
}

inline Buffer& operator>>(Buffer& buf, uint32_t& val)
{
  boost::endian::big_uint32_buf_t big_endian_val;
  buf >> gsl::span(&big_endian_val, 1);
  val = big_endian_val.value();
  return buf;
}

inline Buffer& operator>>(Buffer& buf, uint64_t& val)
{
  boost::endian::big_uint64_buf_t big_endian_val;
  buf >> gsl::span(&big_endian_val, 1);
  val = big_endian_val.value();
  return buf;
}

inline Buffer& operator>>(Buffer& buf, uint256_t& val)
{
  std::array<uint8_t, 32> temp_buf{};
  buf >> gsl::make_span(temp_buf);
  import_bits(val, temp_buf.begin(), temp_buf.end());
  return buf;
}
