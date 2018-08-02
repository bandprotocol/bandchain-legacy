#pragma once

#include <iostream>

#include "abci/abci.pb.h"
#include "net/netapp.h"

class TendermintApplication : public NetApplication
{
public:
private:
  void process(Buffer& read_buffer, Buffer& write_buffer) final
  {
    (void)read_buffer;
    (void)write_buffer;

    int size = 0;
    if (!read_integer(read_buffer, size))
      return;

    abci::Request request;
    request.ParseFromArray(read_buffer.begin(), size);

    if (!request.IsInitialized()) {
      // TODO: log_err
    }

    std::cout << request.DebugString().c_str() << std::endl;
    switch (request.value_case()) {
      case abci::Request::ValueCase::kEcho:;
      case abci::Request::ValueCase::kFlush:
      case abci::Request::ValueCase::kInfo:
      case abci::Request::ValueCase::kSetOption:
      case abci::Request::ValueCase::kInitChain:
      case abci::Request::ValueCase::kQuery:
      case abci::Request::ValueCase::kBeginBlock:
      case abci::Request::ValueCase::kCheckTx:
      case abci::Request::ValueCase::kDeliverTx:
      case abci::Request::ValueCase::kEndBlock:
      case abci::Request::ValueCase::kCommit:
        break;
      default:
        // TODO: log_err
        break;
    }

    // types::Response response;
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

  bool read_integer(Buffer& read_buffer, int& value);

  void write_integer(Buffer& write_buffer, int value);

private:
};
