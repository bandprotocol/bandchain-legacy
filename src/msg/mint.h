#pragma once

#include "msg/base.h"

struct MintMsg : Msg {
  static constexpr MsgType MsgID = MsgType::Mint;

  Address addr;
  BigInt value;
  Hash ident;

  /// As required by base message.
  size_t size() const { return sizeof(MintMsg); }
  Hash hash() const { return sha256(this, size()); }
};
static_assert(sizeof(MintMsg) == sizeof(Msg) + 84, "Invalid TxMsg size");
