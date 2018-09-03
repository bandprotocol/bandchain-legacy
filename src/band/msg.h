#pragma once

#include "inc/essential.h"
#include "util/buffer.h"
#include "util/bytes.h"
#include "util/endian.h"

struct MsgID {
  enum : uint16_t {
    Unset = 0,
    Mint = 1,
    Tx = 2,
  };
};

/// The header of ANY message that will be processed in this BAND blockchain.
struct MsgHdr {
  uint16_t msgid{}; //< The ID from this message
  uint64_t ts{};    //< The timestamp at which this message is broadcasted
  VerifyKey vk{};   //< The verify key of the person who broadcasts this message

  friend Buffer& operator<<(Buffer& buf, const MsgHdr& msg)
  {
    return buf << msg.msgid << msg.ts << msg.vk;
  }

  friend Buffer& operator>>(Buffer& buf, MsgHdr& msg)
  {
    return buf >> msg.msgid >> msg.ts >> msg.vk;
  }
};

template <uint16_t MSG_ID>
struct BaseMsg {
  /// Convenient static member so that the ID can be easily accessed
  static constexpr uint16_t ID = MSG_ID;
};

/// MingMsg allows anyone in the world to mint token to their own account.
/// This message is primarily used for testing and will obviously not be
/// available for use in mainnet.
struct MintMsg : BaseMsg<MsgID::Mint> {
  TokenKey token_key{}; // The token ID to mint
  uint256_t value{};    //< The value to mint

  friend Buffer& operator<<(Buffer& buf, const MintMsg& msg)
  {
    return buf << msg.token_key << msg.value;
  }

  friend Buffer& operator>>(Buffer& buf, MintMsg& msg)
  {
    return buf >> msg.token_key >> msg.value;
  }
};

/// TxMsg allows anyone to send their tokens to another party. The only
/// requirement is that you must have at least that amount of tokens in your
/// blockchain account.
struct TxMsg : BaseMsg<MsgID::Tx> {
  TokenKey token_key{}; // The token ID to send
  Address dest{};       //< The address to send the value to
  uint256_t value{};    //< The value of this transaction

  friend Buffer& operator<<(Buffer& buf, const TxMsg& msg)
  {
    return buf << msg.token_key << msg.dest << msg.value;
  }

  friend Buffer& operator>>(Buffer& buf, TxMsg& msg)
  {
    return buf >> msg.token_key >> msg.dest >> msg.value;
  }
};
