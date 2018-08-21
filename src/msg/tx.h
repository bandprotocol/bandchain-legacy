#pragma once

#include "crypto/sha256.h"
#include "msg/base.h"

struct TxMsg : Msg {
  static constexpr MsgType MsgID = MsgType::Tx;

  big_uint8_t input_count = 0;
  big_uint8_t output_count = 0;

  struct TxInput {
    Hash ident;
    VerifyKey vk;
    Signature sig;
  };

  struct TxOutput {
    Address addr;
    BigInt value;
  };

  /// Return the dynamic size of this transaction message.
  size_t size() const;

  /// Return the sha256 hash of this transaction, with all signatures set to 0.
  /// This is not very efficient and should only be called once per transaction.
  Hash unsig_hash() const;

  gsl::span<TxInput> inputs();
  gsl::span<const TxInput> inputs() const;

  gsl::span<TxOutput> outputs();
  gsl::span<const TxOutput> outputs() const;
};
static_assert(sizeof(TxMsg) == sizeof(Msg) + 2, "Invalid TxMsg size");

////////////////////////////////////////////////////////////////////////////////
inline Hash TxMsg::unsig_hash() const
{
  std::vector<unsigned char> copy_buf(size());
  std::memcpy(copy_buf.data(), this, size());
  auto copy_tx_msg = reinterpret_cast<TxMsg*>(copy_buf.data());
  for (auto& tx_input : copy_tx_msg->inputs()) {
    tx_input.sig = Signature();
  }
  return sha256(copy_buf);
}

inline size_t TxMsg::size() const
{
  const size_t base_size = sizeof(TxMsg);
  const size_t input_size = sizeof(TxMsg::TxInput) * input_count;
  const size_t output_size = sizeof(TxMsg::TxOutput) * output_count;
  return base_size + input_size + output_size;
}

inline gsl::span<TxMsg::TxInput> TxMsg::inputs()
{
  auto base = reinterpret_cast<unsigned char*>(this) + sizeof(TxMsg);
  return {reinterpret_cast<TxInput*>(base), input_count};
}

inline gsl::span<const TxMsg::TxInput> TxMsg::inputs() const
{
  auto base = reinterpret_cast<const unsigned char*>(this) + sizeof(TxMsg);
  return {reinterpret_cast<const TxInput*>(base), input_count};
}

inline gsl::span<TxMsg::TxOutput> TxMsg::outputs()
{
  auto base = reinterpret_cast<unsigned char*>(this) + sizeof(TxMsg);
  auto offset = sizeof(TxMsg::TxInput) * input_count;
  return {reinterpret_cast<TxOutput*>(base + offset), output_count};
}

inline gsl::span<const TxMsg::TxOutput> TxMsg::outputs() const
{
  auto base = reinterpret_cast<const unsigned char*>(this) + sizeof(TxMsg);
  auto offset = sizeof(TxMsg::TxInput) * input_count;
  return {reinterpret_cast<const TxOutput*>(base + offset), output_count};
}
