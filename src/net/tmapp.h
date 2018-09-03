#pragma once

#include "abci/abci.h"
#include "inc/essential.h"
#include "net/netapp.h"

class TendermintApplication : public NetApplication
{
public:
  /// What's the name of the running application?
  virtual std::string get_name() const = 0;

  /// What's the version of the running application?
  virtual std::string get_version() const = 0;

  /// What's the current application's Merkle hash?
  virtual std::string get_current_app_hash() const = 0;

  /// Initialize the blockchain based on the given app data.
  virtual void init(const std::string& init_state) = 0;

  /// Query blockchain state.
  virtual std::string query(const std::string& path,
                            const std::string& data) = 0;

  /// Apply an incoming message. Throw if the message is not valid.
  virtual void check(const std::string& msg_raw) = 0;

  /// Apply an incoming message to the blockchain.
  virtual void apply(const std::string& msg_raw) = 0;

protected:
  uint64_t last_block_height = 0;

private:
  /// Return Tendermint version that this app supports.
  std::string get_tm_version() { return "0.21.0-46369a1a"; }

  void do_info(const RequestInfo&, ResponseInfo&);
  void do_init_chain(const RequestInitChain&);
  void do_query(const RequestQuery&, ResponseQuery&);
  void do_begin_block(const RequestBeginBlock&);
  void do_check_tx(const RequestCheckTx&, ResponseCheckTx&);
  void do_deliver_tx(const RequestDeliverTx&, ResponseDeliverTx&);
  void do_end_block(const RequestEndBlock&, ResponseEndBlock&);
  void do_commit(ResponseCommit&);

  /// Process incoming message and write to the outgoing buffer.
  bool process(Buffer& read_buffer, Buffer& write_buffer) final;

  /// Read a varint encoded integer from the buffer.
  bool read_integer(Buffer& read_buffer, int& value);

  /// Write a varint encoded integer to the buffer.
  void write_integer(Buffer& write_buffer, int value);
};
