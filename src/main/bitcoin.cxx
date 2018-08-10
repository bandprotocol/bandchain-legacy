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
  std::string get_name() const final { return "bitcoin-like"; }
  std::string get_version() const final { return "1.0"; }

  std::string get_current_app_hash() const final
  {
    return std::to_string(tx_count);
  }

  void init(const std::string& init_state) final
  {
    (void)init_state;
    utxos.clear();
    utxos[Ident()] = std::pair(
        Address::from_hex("79e56486055732bc050fea8de33635e6cda17cd1"), 10'000);
    tx_count = 0;
  }

  std::string query(const std::string& path,
                    const std::string& data) const final
  {
    (void)path;
    (void)data;
    std::string ret;
    ret += "tx count: {}\n"_format(tx_count);
    for (auto& [tx_id, tx_value] : utxos) {
      ret += "{}: owner={} value={}\n"_format(tx_id, tx_value.first,
                                              tx_value.second);
    }
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
      if (msg_size < sizeof(TxMsg) + tx_msg.extension_size()) {
        return false;
      }

      uint64_t total_input_value = 0;
      uint64_t total_output_value = 0;
      const Bytes<32> tx_hash = msg_unique_hash(tx_msg);

      for (uint8_t idx = 0; idx < tx_msg.input_cnt; ++idx) {
        const TxMsg::TxInput& tx_input = tx_msg.get_input(idx);
        auto it = utxos.find(tx_input.id);
        if (it == utxos.end()) {
          return false;
        }

        if (ed25519_vk_to_addr(tx_input.vk) != it->second.first) {
          return false;
        }

        if (!ed25519_verify(tx_input.sig, tx_input.vk, tx_hash.data(), 32))
          return false;

        if (total_input_value + it->second.second < total_input_value) {
          return false;
        }
        total_input_value += it->second.second;
      }

      for (uint8_t idx = 0; idx < tx_msg.output_cnt; ++idx) {
        const TxMsg::TxOutput& tx_output = tx_msg.get_output(idx);
        if (total_output_value + tx_output.value < total_output_value) {
          return false;
        }
        total_output_value += tx_output.value;
      }

      if (total_input_value != total_output_value) {
        return false;
      }

      if (apply) {
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

      return true;
    }
    return false;
  }

private:
  std::unordered_map<Ident, std::pair<Address, uint64_t>> utxos;
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
