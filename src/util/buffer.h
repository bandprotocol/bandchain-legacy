#pragma once

#include <vector>

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
  decltype(auto) size_bytes() const { return as_span().size_bytes(); }

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
