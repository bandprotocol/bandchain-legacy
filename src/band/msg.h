#pragma once

#include "inc/essential.h"
#include "util/buffer.h"
#include "util/bytes.h"
#include "util/endian.h"
#include "util/equation.h"

struct MsgID {
  enum : uint16_t {
    Unset = 0,
    Mint = 1,
    Tx = 2,
    Create = 3,
    PurchaseCT = 4,
    SellCT = 5,
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

/// CreateMsg allows anyone to create new community by initializing
/// bonding curve equation.
struct CreateMsg : BaseMsg<MsgID::Create> {
  Curve curve;
  uint256_t max_supply{};

  friend Buffer& operator<<(Buffer& buf, const CreateMsg& msg)
  {
    return buf << msg.curve << msg.max_supply;
  }

  friend Buffer& operator>>(Buffer& buf, CreateMsg& msg)
  {
    return buf >> msg.curve >> msg.max_supply;
  }
};

// PurchaseCTMsg allows anyone to purchase community tokens from contractID
struct PurchaseCTMsg : BaseMsg<MsgID::PurchaseCT> {
  ContractID contract_id{};
  uint256_t amount{};
  uint256_t band_limit{};

  friend Buffer& operator<<(Buffer& buf, const PurchaseCTMsg& msg)
  {
    return buf << msg.contract_id << msg.amount << msg.band_limit;
  }

  friend Buffer& operator>>(Buffer& buf, PurchaseCTMsg& msg)
  {
    return buf >> msg.contract_id >> msg.amount >> msg.band_limit;
  }
};

// SellCTMsg allows anyone to sell community token and take band back.
struct SellCTMsg : BaseMsg<MsgID::SellCT> {
  ContractID contract_id{};
  uint256_t amount{};
  uint256_t band_limit{};

  friend Buffer& operator<<(Buffer& buf, const SellCTMsg& msg)
  {
    return buf << msg.contract_id << msg.amount << msg.band_limit;
  }

  friend Buffer& operator>>(Buffer& buf, SellCTMsg& msg)
  {
    return buf >> msg.contract_id >> msg.amount >> msg.band_limit;
  }
};
