#pragma once

#include "net/netapp.h"

class TendermintApplication : public NetApplication
{
public:
private:
  void process(Buffer& read_buffer, Buffer& write_buffer) final
  {

    // void set_has_echo();
    // void set_has_flush();
    // void set_has_info();
    // void set_has_set_option();
    // void set_has_init_chain();
    // void set_has_query();
    // void set_has_begin_block();
    // void set_has_check_tx();
    // void set_has_deliver_tx();
    // void set_has_end_block();
    // void set_has_commit();
  }

  int read_integer(const Buffer& read_buffer, int& value);

  void write_integer(Buffer& write_buffer, int value);

private:
};
