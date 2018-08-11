#pragma once

#include <boost/endian/arithmetic.hpp>

#include "crypto/sha256.h"
#include "util/bytes.h"
#include "util/enum.h"

using namespace boost::endian;

/// All message types in Band Protocol.
BETTER_ENUM(MsgType, uint16_t, UNSET = 0, TX = 1);

/// Base message type. Any message to the blockchain must be a subclass of this.
struct Msg {
  big_uint16_t msg_type = 0; //< The type of this message
  big_uint64_t msg_ts = 0;   //< Epoch timestamp when this message is created

  /// To be implemented by each of the messages.
  size_t size() const { throw std::runtime_error("size unimplemented"); }

  /// To be implemented by each of the messages.
  Hash hash() const { throw std::runtime_error("hash unimplemented"); }

} __attribute__((packed));

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
