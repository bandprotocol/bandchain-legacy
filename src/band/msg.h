#pragma once

#include <enum/enum.h>

#include "inc/essential.h"
#include "util/buffer.h"
#include "util/bytes.h"
#include "util/endian.h"
#include "util/equation.h"
#include "util/time.h"

BETTER_ENUM(MsgID, uint16_t, Unset = 0, Mint = 1, Tx = 2, CreateContract = 3,
            PurchaseContract = 4, SellContract = 5, SpendToken = 6,
            CreateRevenue = 7)

/// The header of ANY message that will be processed in this BAND blockchain.
struct MsgHdr {
  MsgID msgid{MsgID::Unset}; //< The ID from this message
  uint64_t ts{};  //< The timestamp at which this message is broadcasted
  VerifyKey vk{}; //< The verify key of the person who broadcasts this message

  friend Buffer& operator<<(Buffer& buf, const MsgHdr& msg)
  {
    return buf << msg.msgid << msg.ts << msg.vk;
  }

  friend Buffer& operator>>(Buffer& buf, MsgHdr& msg)
  {
    return buf >> msg.msgid >> msg.ts >> msg.vk;
  }
};

template <MsgID::_enumerated MSG_ID>
struct BaseMsg {
  /// Convenient static member so that the ID can be easily accessed
  static constexpr MsgID ID = MSG_ID;
};

/// MingMsg allows anyone in the world to mint token to their own account.
/// This message is primarily used for testing and will obviously not be
/// available for use in mainnet.
struct MintMsg : BaseMsg<MsgID::Mint> {
  ContractID token_key{}; // The token ID to mint
  uint256_t value{};      //< The value to mint

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
  ContractID token_key{}; // The token ID to send
  AccountID dest{};       //< The address to send the value to
  uint256_t value{};      //< The value of this transaction

  friend Buffer& operator<<(Buffer& buf, const TxMsg& msg)
  {
    return buf << msg.token_key << msg.dest << msg.value;
  }

  friend Buffer& operator>>(Buffer& buf, TxMsg& msg)
  {
    return buf >> msg.token_key >> msg.dest >> msg.value;
  }
};

/// CreateContractMsg allows anyone to create new contract by initializing
/// bonding curve equation and revenue id to collect revenue from redeem token.
struct CreateContractMsg : BaseMsg<MsgID::CreateContract> {
  RevenueID revenue_id{};
  Curve buy_curve;
  Curve sell_curve;
  uint256_t max_supply{};
  bool is_transferable{};
  bool is_discountable{};
  AccountID beneficiary{};

  friend Buffer& operator<<(Buffer& buf, const CreateContractMsg& msg)
  {
    return buf << msg.revenue_id << msg.buy_curve << msg.sell_curve
               << msg.max_supply << msg.is_transferable << msg.is_discountable
               << msg.beneficiary;
  }

  friend Buffer& operator>>(Buffer& buf, CreateContractMsg& msg)
  {
    return buf >> msg.revenue_id >> msg.buy_curve >> msg.sell_curve >>
           msg.max_supply >> msg.is_transferable >> msg.is_discountable >>
           msg.beneficiary;
  }
};

/// PurchaseContractMsg allows anyone to purchase token(s) from contract.
struct PurchaseContractMsg : BaseMsg<MsgID::PurchaseContract> {
  ContractID contract_id{};
  uint256_t value{};
  uint256_t price_limit{};

  friend Buffer& operator<<(Buffer& buf, const PurchaseContractMsg& msg)
  {
    return buf << msg.contract_id << msg.value << msg.price_limit;
  }

  friend Buffer& operator>>(Buffer& buf, PurchaseContractMsg& msg)
  {
    return buf >> msg.contract_id >> msg.value >> msg.price_limit;
  }
};

/// SellContractMsg allows anyone to sell token(s) and take "base" token(s)
/// back.
struct SellContractMsg : BaseMsg<MsgID::SellContract> {
  ContractID contract_id{};
  uint256_t value{};
  uint256_t price_limit{};

  friend Buffer& operator<<(Buffer& buf, const SellContractMsg& msg)
  {
    return buf << msg.contract_id << msg.value << msg.price_limit;
  }

  friend Buffer& operator>>(Buffer& buf, SellContractMsg& msg)
  {
    return buf >> msg.contract_id >> msg.value >> msg.price_limit;
  }
};

struct SpendTokenMsg : BaseMsg<MsgID::SpendToken> {
  ContractID token_key{};
  uint256_t value{};

  friend Buffer& operator<<(Buffer& buf, const SpendTokenMsg& msg)
  {
    return buf << msg.token_key << msg.value;
  }

  friend Buffer& operator>>(Buffer& buf, SpendTokenMsg& msg)
  {
    return buf >> msg.token_key >> msg.value;
  }
};

struct CreateRevenueMsg : BaseMsg<MsgID::CreateRevenue> {
  ContractID base_token_id{};
  AccountID manager{};
  StakeID stake_id{};
  TimePeriod time_period{};
  bool is_private{};

  friend Buffer& operator<<(Buffer& buf, const CreateRevenueMsg& msg)
  {
    return buf << msg.base_token_id << msg.manager << msg.stake_id
               << msg.time_period << msg.is_private;
  }

  friend Buffer& operator>>(Buffer& buf, CreateRevenueMsg& msg)
  {
    return buf >> msg.base_token_id >> msg.manager >> msg.stake_id >>
           msg.time_period >> msg.is_private;
  }
};
