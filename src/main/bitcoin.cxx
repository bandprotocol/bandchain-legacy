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
    Hash hash = state.hash();
    return std::string((const char*)hash.data(), Hash::Size);
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

  void apply(const std::string& msg_data, DryRun dry_run) final
  {
    const size_t msg_size = msg_data.size();
    if (msg_size < sizeof(Msg)) {
      throw std::invalid_argument("Bad message size");
    }

    const Msg& msg = *reinterpret_cast<const Msg*>(msg_data.c_str());
    if (msg.msg_type == MsgType::Tx) {
      if (msg_size < sizeof(TxMsg)) {
        throw std::invalid_argument("Bad message size");
      }

      const TxMsg& tx_msg = static_cast<const TxMsg&>(msg);
      if (msg_size < tx_msg.size()) {
        throw std::invalid_argument("Bad message size");
      }

      uint256_t total_input_value = 0;
      uint256_t total_output_value = 0;
      // TODO
      const Bytes<32> tx_hash; // = tx_msg.hash();

      for (const auto& tx_input : tx_msg.inputs()) {
        const auto& txo = state.find<TxOutput>(tx_input.ident);
        if (!txo.is_spendable(tx_input.vk)) {
          throw std::invalid_argument("Tx is not spendable");
        }

        // TODO
        // if (!ed25519_verify(tx_msg.get_signature(idx), tx_input.vk,
        //                     tx_hash.data(), 32))
        //   throw std::invalid_argument("Bad tx signature");

        if (total_input_value + txo.get_value() < total_input_value) {
          throw std::invalid_argument("overflow");
        }
        total_input_value += txo.get_value();
      }

      for (const auto& tx_output : tx_msg.outputs()) {
        if (total_output_value + tx_output.value.as_uint256() <
            total_output_value) {
          throw std::invalid_argument("overflow");
        }
        total_output_value += tx_output.value.as_uint256();
      }

      if (total_input_value != total_output_value) {
        throw std::invalid_argument("Tx input and output value not match");
      }

      if (dry_run == DryRun::No) {
        for (const auto& tx_input : tx_msg.inputs()) {
          auto& txo = state.find<TxOutput>(tx_input.ident);
          txo.spend();
        }

        Bytes<32> output_tx_nonce = tx_hash;
        for (const auto& tx_output : tx_msg.outputs()) {
          output_tx_nonce = sha256(output_tx_nonce);
          state.add(std::make_unique<TxOutput>(
              tx_output.addr, tx_output.value.as_uint256(), output_tx_nonce));
        }
        ++tx_count;
      }
    }
    throw std::invalid_argument("invalid message type");
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
