#include <unordered_map>

#include "crypto/ed25519.h"
#include "crypto/sha256.h"
#include "msg/tx.h"
#include "net/server.h"
#include "net/tmapp.h"
#include "state/merkle.h"
#include "state/output.h"

class BitcoinApplication : public TendermintApplication
{
public:
  std::string get_name() const final { return "bitcoin-like"; }
  std::string get_version() const final { return "1.0"; }

  std::string get_current_app_hash() const final
  {
    return std::to_string(tx_count);
  }

  void init(const std::string& init_state) final
  {
    (void)init_state;
    // utxos.clear();

    Address me = Address::from_hex("79e56486055732bc050fea8de33635e6cda17cd1");
    state.add(std::make_unique<TxOutput>(me, 10'000, Hash()));
    tx_count = 0;
  }

  std::string query(const std::string& path,
                    const std::string& data) const final
  {
    (void)path;
    (void)data;
    std::string ret;
    ret += "tx count: {}\n{}\n"_format(tx_count, state);
    return ret;
  }

  bool check(const std::string& msg_data, bool apply) final
  {
    const size_t msg_size = msg_data.size();
    if (msg_size < sizeof(Msg)) {
      return false;
    }

    const Msg& msg = *reinterpret_cast<const Msg*>(msg_data.c_str());
    if (msg.msg_type == MsgType::TX) {
      if (msg_size < sizeof(TxMsg)) {
        return false;
      }

      const TxMsg& tx_msg = static_cast<const TxMsg&>(msg);
      if (msg_size < tx_msg.size()) {
        return false;
      }

      uint256_t total_input_value = 0;
      uint256_t total_output_value = 0;
      const Bytes<32> tx_hash = tx_msg.hash();

      for (uint8_t idx = 0; idx < tx_msg.input_cnt; ++idx) {
        const TxMsg::TxInput& tx_input = tx_msg.get_input(idx);

        const Hashable& data = state.find(tx_input.id);
        if (data.type() != HashableType::TxOutput) {
          return false;
        }

        const TxOutput& txo = static_cast<const TxOutput&>(data);
        if (!txo.spendable(tx_input.vk)) {
          return false;
        }

        if (!ed25519_verify(tx_msg.get_signature(idx), tx_input.vk,
                            tx_hash.data(), 32))
          return false;

        if (total_input_value + txo.get_value() < total_input_value) {
          return false;
        }
        total_input_value += txo.get_value();
      }

      for (uint8_t idx = 0; idx < tx_msg.output_cnt; ++idx) {
        const TxMsg::TxOutput& tx_output = tx_msg.get_output(idx);
        if (total_output_value + tx_output.value.as_uint256() <
            total_output_value) {
          return false;
        }
        total_output_value += tx_output.value.as_uint256();
      }

      if (total_input_value != total_output_value) {
        return false;
      }

      if (apply) {
        for (uint8_t idx = 0; idx < tx_msg.input_cnt; ++idx) {
          const TxMsg::TxInput& tx_input = tx_msg.get_input(idx);
          TxOutput& txo = static_cast<TxOutput&>(state.find(tx_input.id));
          txo.spend();
        }

        Bytes<32> output_tx_nonce = tx_hash;
        for (uint8_t idx = 0; idx < tx_msg.output_cnt; ++idx) {
          const TxMsg::TxOutput& tx_output = tx_msg.get_output(idx);
          output_tx_nonce = sha256(output_tx_nonce);
          state.add(std::make_unique<TxOutput>(
              tx_output.addr, tx_output.value.as_uint256(), output_tx_nonce));
        }
        ++tx_count;
      }

      return true;
    }
    return false;
  }

private:
  MerkleTree state;
  int tx_count = 0;
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
