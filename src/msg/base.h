#pragma once

#include "crypto/sha256.h"
#include "util/bytes.h"
#include "util/enum.h"

/// All message types in Band Protocol.
BETTER_ENUM(MsgType, uint16_t, Unset = 0, Mint = 1, Tx = 2);

/// Base message type. Any message to the blockchain must be a subclass of this.
struct Msg {
  big_uint16_t msg_type = 0; //< The type of this message
  big_uint16_t msg_size = 0; //< The size of this message
  big_uint64_t msg_ts = 0;   //< Epoch timestamp when this message is created

  /// Must be overridden if Msg has dynamic size.
  size_t size() const;
};
static_assert(sizeof(Msg) == 12, "Invalid Msg Base size");

namespace base
{
template <typename T>
bool msg_size_valid(const T& msg, size_t size)
{
  if (size != msg.msg_size)
    return false;
  if (size < sizeof(T))
    return false;
  if (size < msg_size(msg))
    return false;
  return true;
}

/// Return the size of the message.
template <typename T>
size_t msg_size(const T& msg)
{
  if constexpr (!std::is_same_v<decltype(&T::size), decltype(&Msg::size)>) {
    return msg.size();
  }
  return sizeof(T);
}

/// Return the hex representation of the message. Not meant to be efficient.
template <typename T>
std::string msg_hex(const T& msg)
{
  std::string hex;
  for (size_t idx = 0; idx < msg.size(); ++idx) {
    hex += "{:02x}"_format(reinterpret_cast<const unsigned char*>(&msg)[idx]);
  }
  return hex;
}
} // namespace base
