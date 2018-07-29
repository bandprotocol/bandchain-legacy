#pragma once

#include "util/buffer.h"

class Application
{
public:
  //! TODO: comment
  virtual ~Application(){};

  //! TODO: comment
  virtual void process(Buffer& read_buffer, Buffer& write_buffer) = 0;
};
