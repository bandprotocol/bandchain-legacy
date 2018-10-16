#pragma once

#include <deque>
#include <type_traits>

#include "inc/essential.h"
#include "util/string.h"

class Buffer
{
public:
  /// Initialize an empty buffer.
  Buffer();

  /// Initialize the buffer from the given data. Perform memcpy under the hood.
  template <typename T>
  Buffer(gsl::span<T> data);

  template <typename T>
  static T deserialize(const std::string& raw_data)
  {
    Buffer buf(gsl::make_span(raw_data));
    return buf.read_all<T>();
  }

  /// Serialize the given data into raw string. Use operator<< internally.
  template <typename T>
  static std::string serialize(const T& data)
  {
    Buffer buf;
    buf << data;
    return std::string((const char*)&buf.buf.front(), buf.buf.size());
  }

  /// Read this buffer as the given type.
  template <typename T>
  T read();

  /// Similar to above, but throw if the read fails or the buffer is not empty
  /// after the read is complete.
  template <typename T>
  T read_all();

  /// Expose this data as a read-only span. Note that if this is destroyed,
  /// the span will become invalid.
  gsl::span<const std::byte> as_span() const { return gsl::make_span(buf); }

  Buffer& operator<<(std::byte val)
  {
    buf.push_back(val);
    return *this;
  }

  Buffer& operator>>(std::byte& val)
  {
    val = buf.front();
    buf.erase(buf.begin());
    return *this;
  }

  Buffer& operator<<(Buffer& data)
  {
    buf.insert(buf.end(), data.buf.begin(), data.buf.end());
    return *this;
  }

  Buffer& operator>>(Buffer& data)
  {
    data.buf.insert(data.buf.end(), buf.begin(), buf.end());
    return *this;
  }

  template <typename T>
  friend Buffer& operator<<(Buffer& buf, gsl::span<T> data)
  {
    auto data_bytes = gsl::as_bytes(data);
    buf.buf.insert(buf.buf.end(), data_bytes.begin(), data_bytes.end());
    return buf;
  }

  template <typename T>
  friend Buffer& operator>>(Buffer& buf, gsl::span<T> data)
  {
    if (buf.size_bytes() < data.size_bytes())
      throw Error("Buffer parse error");
    std::memcpy(data.data(), &buf.buf.front(), data.size_bytes());
    buf.consume(data.size_bytes());
    return buf;
  }

  /// Return whether this buffer is empty
  bool empty() const { return buf.empty(); }

  /// Return the size of this buffer in bytes.
  gsl::span<const std::byte>::size_type size_bytes() const
  {
    return as_span().size_bytes();
  }

  /// Clear the content in this buffer.
  void clear() { buf.clear(); }

  /// Consume the first length bytes of this buffer.
  void consume(size_t length)
  {
    // TODO: Make this be more efficient
    buf.erase(buf.begin(), buf.begin() + length);
  }

  std::string to_string() const { return bytes_to_hex(as_span()); }

  std::string to_raw_string() const
  {
    return std::string((const char*)&buf.front(), buf.size());
  }

private:
  std::vector<std::byte> buf;
};

template <typename T>
Buffer::Buffer(gsl::span<T> data)
{
  buf.resize(data.size_bytes());
  std::memcpy(&(*buf.begin()), data.data(), data.size_bytes());
}

template <typename T>
T Buffer::read()
{
  T result;
  *this >> result;
  return result;
}

template <typename T>
T Buffer::read_all()
{
  T result = read<T>();
  if (!empty())
    throw Error("Buffer::read_all does not fully consume the buffer");
  return result;
}

template <typename T, typename std::enable_if_t<std::is_enum_v<T>, int> = 0>
Buffer& operator<<(Buffer& buf, T val)
{
  return buf << static_cast<std::underlying_type_t<T>>(val);
}

template <typename T, typename std::enable_if_t<std::is_enum_v<T>, int> = 0>
Buffer& operator>>(Buffer& buf, T& val)
{
  std::underlying_type_t<T> _val;
  buf >> _val;
  val = T(_val);
  return buf;
}

template <typename T, typename std::enable_if_t<
                          std::is_integral_v<typename T::_integral>, int> = 0>
Buffer& operator<<(Buffer& buf, T val)
{
  return buf << val._to_integral();
}

template <typename T, typename std::enable_if_t<
                          std::is_integral_v<typename T::_integral>, int> = 0>
Buffer& operator>>(Buffer& buf, T& val)
{
  typename T::_integral _val;
  buf >> _val;
  val = T::_from_integral(_val);
  return buf;
}

template <typename T>
void varint_encode(Buffer& buf, T val)
{
  while (true) {
    std::byte towrite{uint8_t(val & 0x7F)};
    val >>= 7;
    if (val != 0) {
      buf << (towrite | std::byte{0x80});
    } else {
      buf << towrite;
      break;
    }
  }
}

template <typename T>
void varint_decode(Buffer& buf, T& val)
{
  val = 0;

  int idx = 0;
  for (const auto raw_byte : buf.as_span()) {
    int byte = std::to_integer<int>(raw_byte);
    val |= T(byte & 0x7F) << (7 * idx);
    if (!(byte & 0x80)) {
      buf.consume(1 + idx);
      return;
    }

    ++idx;
  }
  throw Error("Invalid varint decode");
}

template <typename T, typename... Args>
void add_buffer(Buffer& buf, T value, Args... others)
{
  buf << value;
  if constexpr (sizeof...(Args) > 0) {
    add_buffer(buf, others...);
  }
}

Buffer& operator<<(Buffer& buf, bool val);
Buffer& operator>>(Buffer& buf, bool& val);

Buffer& operator<<(Buffer& buf, uint8_t val);
Buffer& operator>>(Buffer& buf, uint8_t& val);

Buffer& operator<<(Buffer& buf, uint16_t val);
Buffer& operator>>(Buffer& buf, uint16_t& val);

Buffer& operator<<(Buffer& buf, uint32_t val);
Buffer& operator>>(Buffer& buf, uint32_t& val);

Buffer& operator<<(Buffer& buf, uint64_t val);
Buffer& operator>>(Buffer& buf, uint64_t& val);

Buffer& operator<<(Buffer& buf, const uint256_t& val);
Buffer& operator>>(Buffer& buf, uint256_t& val);

Buffer& operator<<(Buffer& buf, const std::string& val);
Buffer& operator>>(Buffer& buf, std::string& val);
