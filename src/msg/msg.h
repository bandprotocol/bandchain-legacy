#pragma once

#include "crypto/ed25519.h"
#include "util/buffer.h"
#include "util/bytes.h"

enum class MsgID : uint16_t {
  Unset = 0,
  Mint = 1,
  Tx = 2,
};

/// The header of ANY message that will be processed in this BAND blockchain.
struct MsgHdr {
  MsgID msgid{};    //< The ID from this message
  uint64_t ts{};    //< The timestamp at which this message is broadcasted
  uint64_t nonce{}; //< The nonce value per account to prevent replay attack
  VerifyKey vk{};   //< The verify key of the person who broadcasts this message
  Signature sig{};  //< The cryptographic signature of this message

  friend Buffer& operator<<(Buffer& buf, const MsgHdr& msg)
  {
    return buf << msg.msgid << msg.ts << msg.nonce << msg.vk << msg.sig;
  }

  friend Buffer& operator>>(Buffer& buf, MsgHdr& msg)
  {
    return buf >> msg.msgid >> msg.ts >> msg.nonce >> msg.vk >> msg.sig;
  }

  bool verify_signature(gsl::span<std::byte> extra) const
  {
    Buffer verify_buffer;
    verify_buffer << msgid << ts << nonce << extra;
    return ed25519_verify(sig, vk, verify_buffer);
  }
};

template <MsgID MSG_ID>
struct BaseMsg {
  /// Convenient static member so that the ID can be easily accessed
  static constexpr MsgID ID = MSG_ID;
};

/// MingMsg allows anyone in the world to mint token to their own account.
/// This message is primarily used for testing and will obviously not be
/// available for use in mainnet.
struct MintMsg : BaseMsg<MsgID::Mint> {
  uint256_t value{}; //< The value to mint

  friend Buffer& operator<<(Buffer& buf, const MintMsg& msg)
  {
    return buf << msg.value;
  }

  friend Buffer& operator>>(Buffer& buf, MintMsg& msg)
  {
    return buf >> msg.value;
  }
};

/// TxMsg allows anyone to send their tokens to another party. The only
/// requirement is that you must have at least that amount of tokens in your
/// blockchain account.
struct TxMsg : BaseMsg<MsgID::Tx> {
  Address dest{};    //< The address to send the value to
  uint256_t value{}; //< The value of this transaction

  friend Buffer& operator<<(Buffer& buf, const TxMsg& msg)
  {
    return buf << msg.dest << msg.value;
  }

  friend Buffer& operator>>(Buffer& buf, TxMsg& msg)
  {
    return buf >> msg.dest >> msg.value;
  }
};
