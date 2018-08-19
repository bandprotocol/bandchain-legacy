#pragma once

#include "msg/base.h"

struct TxMsg : Msg {
  static constexpr MsgType MsgID = MsgType::Tx;

  big_uint8_t input_cnt = 0;
  big_uint8_t output_cnt = 0;

  struct TxInput {
    Hash ident;
    VerifyKey vk;
  };

  struct TxOutput {
    Address addr;
    BigInt value;
  };

  struct TxSig {
    Signature sig;
  };

  /// As required by base message.
  size_t size() const;
  Hash hash() const;

  /// Return the idx^th input of this transaction.
  TxInput& get_input(uint8_t idx);
  const TxInput& get_input(uint8_t idx) const;

  /// Return the idx^th output of this transaction.
  TxOutput& get_output(uint8_t idx);
  const TxOutput& get_output(uint8_t idx) const;

  /// Return the idx^th input signature of this transaction.
  Signature& get_signature(uint8_t idx);
  const Signature& get_signature(uint8_t idx) const;
};
static_assert(sizeof(TxMsg) == sizeof(Msg) + 2, "Invalid TxMsg size");
