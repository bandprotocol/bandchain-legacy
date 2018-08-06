#pragma once

#include "util/enum.h"

struct MsgID {
  enum : int16_t {
    INVALID = 0,
    TRANSFER = 1,
  };
};

struct Msg {
  uint16_t msg_id = 0;   //<
  uint16_t msg_size = 0; //<

  uint64_t msg_ts = 0;
};

// struct TransferMsg : public Msg {
//   char
// };
