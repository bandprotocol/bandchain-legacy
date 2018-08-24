#pragma once

#include "msg/base.h"

struct MintMsg : MsgBase<MintMsg, MsgID::Mint> {
  Address addr;
  BigInt value;
  Hash ident;
};
static_assert(sizeof(MintMsg) == sizeof(MsgBaseVoid) + 84,
              "Invalid MingMsg size");
