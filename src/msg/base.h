#pragma once

#include <boost/endian/arithmetic.hpp>

#include "crypto/sha256.h"

using boost::endian::big_uint16_t;
using boost::endian::big_uint32_t;
using boost::endian::big_uint64_t;
using boost::endian::big_uint8_t;

/// All message types in Band Protocol.
enum class MsgID : uint16_t {
  Unset = 0,
  Mint = 1,
  Tx = 2,
};

/// Base message type. Any message to the blockchain must be a subclass of this.
template <typename Msg, MsgID _MsgID>
struct MsgBase {
  static constexpr MsgID ID = _MsgID;

  big_uint16_t msgid = 0; //< The id of this message
  big_uint16_t size = 0;  //< The size of this message
  big_uint64_t ts = 0;    //< Epoch timestamp when this message is created

  operator gsl::span<const std::byte>() const
  {
    return {reinterpret_cast<const std::byte*>(this), size};
  }

  MsgID id() const { return MsgID(uint16_t(msgid)); }

  size_t msg_size() const
  {
    // Must be overridden if Msg has dynamic size.
    static_assert(std::is_base_of_v<MsgBase, Msg>);
    return sizeof(Msg);
  }

  std::string hex() const
  {
    static_assert(std::is_base_of_v<MsgBase, Msg>);
    bytes_to_hex(this);
  }
};

struct MsgBaseVoid : MsgBase<MsgBaseVoid, MsgID::Unset> {
  template <typename T, typename = std::enable_if_t<
                            std::is_base_of_v<MsgBase<T, T::ID>, T>>>
  const T& as() const
  {
    if (size < sizeof(T))
      throw Error("");

    const T& msg = reinterpret_cast<const T&>(*this);
    if (size < msg.msg_size())
      throw Error("");

    return msg;
  }
};

static_assert(sizeof(MsgBaseVoid) == 12, "Invalid Msg Base size");
