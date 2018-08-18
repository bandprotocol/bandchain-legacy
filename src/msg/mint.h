#pragma once

#include "msg/base.h"

struct MintMsg : Msg {
  static constexpr MsgType MsgID = MsgType::MINT;

  Address addr;
  BigInt value;

  /// As required by base message.
  size_t size() const { return sizeof(MintMsg); }
  Hash hash() const { return sha256(this, size()); }
};
