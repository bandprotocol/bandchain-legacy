#include "net/server.h"
#include "net/tmapp.h"

class CounterApplication : public TendermintApplication
{
public:
  virtual void do_info(const RequestInfo& req, ResponseInfo& res)
  {
    if (req.version() != "0.17.1") {
      throw "Invalid Version";
    }
    res.set_data("counter");
    res.set_version("n/a");
  }

  virtual void do_init_chain(const RequestInitChain&, ResponseInitChain&)
  {
    tx_count = 0;
    last_block_height = 0;
  }

  virtual void do_query(const RequestQuery&, ResponseQuery& res)
  {
    res.set_code(0);
    res.set_value(std::to_string(tx_count));
    res.set_height(last_block_height);
  }

  virtual void do_begin_block(const RequestBeginBlock&, ResponseBeginBlock&)
  {
    // Noop
  }

  virtual void do_check_tx(const RequestCheckTx&, ResponseCheckTx& res)
  {
    res.set_code(0);
  }

  virtual void do_deliver_tx(const RequestDeliverTx& req,
                             ResponseDeliverTx& res)
  {
    if (std::stoi(req.tx()) != tx_count + 1) {
      res.set_code(1);
      return;
    }
    ++tx_count;
    res.set_code(0);
  }

  virtual void do_end_block(const RequestEndBlock&, ResponseEndBlock&)
  {
    // Noop
  }

  virtual void do_commit(const RequestCommit&, ResponseCommit& res)
  {
    res.set_data(std::to_string(tx_count));
  }

private:
  int tx_count = 0;
  int last_block_height = 0;
};

int main()
{
  CounterApplication app;
  boost::asio::io_service service;

  Server server(service, app, 46658);
  server.start();

  service.run();
  return 0;
}
