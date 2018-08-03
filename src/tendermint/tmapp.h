#pragma once

#include "abci/abci.h"
#include "net/netapp.h"

class TendermintApplication : public NetApplication
{
public:
  virtual void do_info(const RequestInfo&, ResponseInfo&) {}
  virtual void do_set_option(const RequestSetOption&, ResponseSetOption&) {}
  virtual void do_init_chain(const RequestInitChain&, ResponseInitChain&) {}
  virtual void do_query(const RequestQuery&, ResponseQuery&) {}
  virtual void do_begin_block(const RequestBeginBlock&, ResponseBeginBlock&) {}
  virtual void do_check_tx(const RequestCheckTx&, ResponseCheckTx&) {}
  virtual void do_deliver_tx(const RequestDeliverTx&, ResponseDeliverTx&) {}
  virtual void do_end_block(const RequestEndBlock&, ResponseEndBlock&) {}
  virtual void do_commit(const RequestCommit&, ResponseCommit&) {}

private:
  bool process(Buffer& read_buffer, Buffer& write_buffer) final
  {
    int size = 0;
    if (!read_integer(read_buffer, size))
      return false;

    Request req;
    Response res;

    req.ParseFromArray(read_buffer.begin(), size);
    read_buffer.consume(size);

    switch (req.value_case()) {
      case abci::Request::ValueCase::kEcho:
        res.mutable_echo()->set_message(req.echo().message());
        break;
      case abci::Request::ValueCase::kFlush:
        res.mutable_flush();
        break;
      case abci::Request::ValueCase::kInfo:
        do_info(req.info(), *res.mutable_info());
        break;
      case abci::Request::ValueCase::kSetOption:
        do_set_option(req.set_option(), *res.mutable_set_option());
        break;
      case abci::Request::ValueCase::kInitChain:
        do_init_chain(req.init_chain(), *res.mutable_init_chain());
        break;
      case abci::Request::ValueCase::kQuery:
        do_query(req.query(), *res.mutable_query());
        break;
      case abci::Request::ValueCase::kBeginBlock:
        do_begin_block(req.begin_block(), *res.mutable_begin_block());
        break;
      case abci::Request::ValueCase::kCheckTx:
        do_check_tx(req.check_tx(), *res.mutable_check_tx());
        break;
      case abci::Request::ValueCase::kDeliverTx:
        do_deliver_tx(req.deliver_tx(), *res.mutable_deliver_tx());
        break;
      case abci::Request::ValueCase::kEndBlock:
        do_end_block(req.end_block(), *res.mutable_end_block());
        break;
      case abci::Request::ValueCase::kCommit:
        do_commit(req.commit(), *res.mutable_commit());
        break;
      default:
        // TODO: log_err
        break;
    }

    std::cout << req.DebugString().c_str() << std::endl;
    std::cout << res.DebugString().c_str() << std::endl;

    size_t write_size = res.ByteSizeLong();
    write_integer(write_buffer, write_size);
    std::byte* write_location = write_buffer.reserve(write_size);
    res.SerializeToArray(write_location, write_size);

    return true;
  }

  //! Read a varint encoded integer from the buffer
  bool read_integer(Buffer& read_buffer, int& value);

  //! Write a varint encoded integer to the buffer
  void write_integer(Buffer& write_buffer, int value);
};
