#pragma once

#include "crypto/sha256.h"
#include "util/bytes.h"

struct MsgType {
  enum : int16_t {
    INVALID = 0,
    TX = 1,
  };

  uint16_t msg_id;
};

/// Start packing structs.
#pragma pack(push, 1)

struct Msg {
  uint16_t msg_type = 0; //< The type of this message
  uint64_t msg_ts = 0;   //< Epoch timestamp when this message is created

  size_t extension_size() const { return 0; }
};

struct TxMsg : public Msg {
  static constexpr uint16_t MsgID = MsgType::TX;

  uint8_t input_cnt = 0;
  uint8_t output_cnt = 0;

  struct TxInput {
    Bytes<32> ident;
    Bytes<12> addr_suffix;
    Bytes<64> sig;
  };

  struct TxOutput {
    Bytes<20> addr;
    uint64_t amount = 0;
  };

  size_t extension_size() const
  {
    return input_cnt * sizeof(TxInput) + output_cnt * sizeof(TxOutput);
  }

  const TxInput& get_input(uint8_t idx) const
  {
    const std::byte* ext =
        reinterpret_cast<const std::byte*>(this) + sizeof(TxMsg);
    size_t offset = idx * sizeof(TxInput);
    return *reinterpret_cast<const TxInput*>(ext + offset);
  }

  const TxOutput& get_output(uint8_t idx) const
  {
    const std::byte* ext =
        reinterpret_cast<const std::byte*>(this) + sizeof(TxMsg);
    size_t offset = input_cnt * sizeof(TxInput) + idx * sizeof(TxOutput);
    return *reinterpret_cast<const TxOutput*>(ext + offset);
  }

  TxInput& get_input(uint8_t idx)
  {
    std::byte* ext = reinterpret_cast<std::byte*>(this) + sizeof(TxMsg);
    size_t offset = idx * sizeof(TxInput);
    return *reinterpret_cast<TxInput*>(ext + offset);
  }

  TxOutput& get_output(uint8_t idx)
  {
    std::byte* ext = reinterpret_cast<std::byte*>(this) + sizeof(TxMsg);
    size_t offset = input_cnt * sizeof(TxInput) + idx * sizeof(TxOutput);
    return *reinterpret_cast<TxOutput*>(ext + offset);
  }
};

/// Stop packing structs.
#pragma pack(pop)

template <typename T>
std::string msg_as_hex(const T& msg)
{
  std::string ret;
  for (size_t idx = 0; idx < sizeof(T) + msg.extension_size(); ++idx) {
    ret += "{:02x}"_format(reinterpret_cast<const unsigned char*>(&msg)[idx]);
  }
  return ret;
}

template <typename T>
Bytes<32> msg_unique_hash(const T& msg)
{
  return sha256(reinterpret_cast<const std::byte*>(&msg), sizeof(T));
}
