#pragma once

#include "util/buffer.h"

class NetApplication
{
public:
  virtual ~NetApplication() {}

  //! Process an incoming message and write outgoing message(s) to buffer.
  virtual bool process(Buffer& read_buffer, Buffer& write_buffer) = 0;
};
