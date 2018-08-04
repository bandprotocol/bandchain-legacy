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
  bool process(Buffer& read_buffer, Buffer& write_buffer) final;

  //! Read a varint encoded integer from the buffer.
  bool read_integer(Buffer& read_buffer, int& value);

  //! Write a varint encoded integer to the buffer.
  void write_integer(Buffer& write_buffer, int value);
};
