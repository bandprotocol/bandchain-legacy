#include <unordered_map>

#include "crypto/ed25519.h"
#include "crypto/sha256.h"
#include "net/server.h"
#include "net/tmapp.h"
#include "state/output.h"
#include "tx/message.h"

class BitcoinApplication : public TendermintApplication
{
public:
  virtual void do_info(const RequestInfo& req, ResponseInfo& res)
  {
    if (req.version() != "0.17.1") {
      throw "Invalid Version";
    }
    res.set_data("bitcoin");
    res.set_version("1.0");
  }

  virtual void do_init_chain(const RequestInitChain&, ResponseInitChain&)
  {
    utxos.clear();
    last_block_height = 0;
  }

  virtual void do_query(const RequestQuery&, ResponseQuery& res)
  {
    std::string ret;
    for (auto& [tx_id, tx_value] : utxos) {
      ret += "{}: owner={} amount={}\n"_format(tx_id, tx_value.first,
                                               tx_value.second);
    }

    res.set_value(ret);
    res.set_code(0);
    res.set_height(last_block_height);
  }

  virtual void do_begin_block(const RequestBeginBlock&, ResponseBeginBlock&)
  {
    // Noop
  }

  virtual void do_check_tx(const RequestCheckTx&, ResponseCheckTx& res)
  {
    // Noop
    res.set_code(0);
  }

  virtual void do_deliver_tx(const RequestDeliverTx& req,
                             ResponseDeliverTx& res)
  {

    const std::string& msg_data = req.tx();
    const size_t msg_size = msg_data.size();

    if (msg_size < sizeof(Msg)) {
      res.set_code(1);
      return;
    }

    const Msg& msg = *reinterpret_cast<const Msg*>(msg_data.c_str());
    if (msg.msg_type == MsgType::TX) {
      if (msg_size < sizeof(TxMsg)) {
        res.set_code(1);
        return;
      }

      const TxMsg& tx_msg = static_cast<const TxMsg&>(msg);
      if (msg_size < sizeof(TxMsg) + tx_msg.extension_size()) {
        res.set_code(1);
        return;
      }

      uint64_t total_input_amount = 0;
      uint64_t total_output_amount = 0;
      const Bytes<32> tx_hash = msg_unique_hash(tx_msg);

      for (uint8_t idx = 0; idx < tx_msg.input_cnt; ++idx) {
        const TxMsg::TxInput& tx_input = tx_msg.get_input(idx);
        auto it = utxos.find(tx_input.ident);
        if (it == utxos.end()) {
          res.set_code(1);
          return;
        }

        if (!ed25519_verify(tx_input.sig,
                            it->second.first + tx_input.addr_suffix,
                            tx_hash.data(), 32)) {
          res.set_code(1);
          return;
        }

        if (total_input_amount + it->second.second < total_input_amount) {
          res.set_code(1);
          return;
        }
        total_input_amount += it->second.second;
      }

      for (uint8_t idx = 0; idx < tx_msg.output_cnt; ++idx) {
        const TxMsg::TxOutput& tx_output = tx_msg.get_output(idx);
        if (total_output_amount + tx_output.amount < total_output_amount) {
          res.set_code(1);
          return;
        }
        total_output_amount += tx_output.amount;
      }

      if (total_input_amount != total_output_amount) {
        res.set_code(1);
        return;
      }

      for (uint8_t idx = 0; idx < tx_msg.input_cnt; ++idx) {
        const TxMsg::TxInput& tx_input = tx_msg.get_input(idx);
        utxos.erase(tx_input.ident);
      }

      Bytes<32> output_tx_hash = tx_hash;
      for (uint8_t idx = 0; idx < tx_msg.output_cnt; ++idx) {
        const TxMsg::TxOutput& tx_output = tx_msg.get_output(idx);
        output_tx_hash = sha256(output_tx_hash.data(), 32);
        utxos[output_tx_hash] = std::pair(tx_output.addr, tx_output.amount);
      }

      res.set_code(0);
      return;
    } else {
      res.set_code(1);
      return;
    }
  }

  virtual void do_end_block(const RequestEndBlock&, ResponseEndBlock&)
  {
    // Noop
  }

  virtual void do_commit(const RequestCommit&, ResponseCommit& res)
  {
    ++last_block_height;
    res.set_data(std::to_string(last_block_height));
  }

private:
  std::unordered_map<Bytes<32>, std::pair<Bytes<20>, uint64_t>> utxos;
  int last_block_height = 0;
};

int main()
{
  BitcoinApplication app;
  boost::asio::io_service service;

  std::unordered_set<Bytes<32>> utxos;
  utxos.insert(Bytes<32>::from_hex(
      "be0cba0e63b4ddd47d9866dc0e71b81520c83a20a54323d2350588db86f0fa85"));

  Server server(service, app, 46658);
  server.start();

  service.run();
  return 0;
}
