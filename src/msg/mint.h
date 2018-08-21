#pragma once

#include "msg/base.h"

struct MintMsg : Msg {
  static constexpr MsgType MsgID = MsgType::Mint;

  Address addr;
  BigInt value;
  Hash ident;
};
static_assert(sizeof(MintMsg) == sizeof(Msg) + 84, "Invalid TxMsg size");
