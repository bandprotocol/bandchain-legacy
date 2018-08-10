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
    res.set_data("bitcoin-like");
    res.set_version("1.0");
  }

  virtual void do_init_chain(const RequestInitChain&, ResponseInitChain&)
  {
    utxos.clear();
    utxos[Ident()] = std::pair(
        Address::from_hex("79e56486055732bc050fea8de33635e6cda17cd1"), 10'000);
    last_block_height = 0;
    tx_count = 0;
  }

  virtual void do_query(const RequestQuery&, ResponseQuery& res)
  {
    std::string ret;
    ret += "tx count: {}\n"_format(tx_count);
    for (auto& [tx_id, tx_value] : utxos) {
      ret += "{}: owner={} value={}\n"_format(tx_id, tx_value.first,
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

  template <typename Req, typename Res>
  void process_tx(const Req& req, Res& res, bool mutate)
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

      uint64_t total_input_value = 0;
      uint64_t total_output_value = 0;
      const Bytes<32> tx_hash = msg_unique_hash(tx_msg);

      for (uint8_t idx = 0; idx < tx_msg.input_cnt; ++idx) {
        const TxMsg::TxInput& tx_input = tx_msg.get_input(idx);
        auto it = utxos.find(tx_input.id);
        if (it == utxos.end()) {
          res.set_code(1);
          return;
        }

        if (ed25519_vk_to_addr(tx_input.vk) != it->second.first) {
          res.set_code(1);
          return;
        }

        if (!ed25519_verify(tx_input.sig, tx_input.vk, tx_hash.data(), 32)) {
          res.set_code(1);
          return;
        }

        if (total_input_value + it->second.second < total_input_value) {
          res.set_code(1);
          return;
        }
        total_input_value += it->second.second;
      }

      for (uint8_t idx = 0; idx < tx_msg.output_cnt; ++idx) {
        const TxMsg::TxOutput& tx_output = tx_msg.get_output(idx);
        if (total_output_value + tx_output.value < total_output_value) {
          res.set_code(1);
          return;
        }
        total_output_value += tx_output.value;
      }

      if (total_input_value != total_output_value) {
        res.set_code(1);
        return;
      }

      if (mutate) {
        for (uint8_t idx = 0; idx < tx_msg.input_cnt; ++idx) {
          const TxMsg::TxInput& tx_input = tx_msg.get_input(idx);
          utxos.erase(tx_input.id);
        }

        Bytes<32> output_tx_hash = tx_hash;
        for (uint8_t idx = 0; idx < tx_msg.output_cnt; ++idx) {
          const TxMsg::TxOutput& tx_output = tx_msg.get_output(idx);
          output_tx_hash = sha256(output_tx_hash.data(), 32);
          utxos[output_tx_hash] = std::pair(tx_output.addr, tx_output.value);
        }
        ++tx_count;
      }

      res.set_code(0);
      return;
    } else {
      res.set_code(1);
      return;
    }
  }

  virtual void do_check_tx(const RequestCheckTx& req, ResponseCheckTx& res)
  {
    process_tx(req, res, false);
  }

  virtual void do_deliver_tx(const RequestDeliverTx& req,
                             ResponseDeliverTx& res)
  {
    process_tx(req, res, true);
  }

  virtual void do_end_block(const RequestEndBlock&, ResponseEndBlock&)
  {
    // Noop
  }

  virtual void do_commit(const RequestCommit&, ResponseCommit& res)
  {
    ++last_block_height;
    res.set_data(std::to_string(tx_count));
  }

private:
  std::unordered_map<Ident, std::pair<Address, uint64_t>> utxos;
  int tx_count = 0;
  int last_block_height = 0;
};

int main()
{
  BitcoinApplication app;
  boost::asio::io_service service;

  Server server(service, app, 46658);
  server.start();

  service.run();
  return 0;
}
