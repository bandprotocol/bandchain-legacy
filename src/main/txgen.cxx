#include <chrono>
#include <iostream>
#include <vector>

#include "crypto/ed25519.h"
#include "tx/message.h"

int main()
{
  char buffer[1 << 16];
  TxMsg& msg = *(new (buffer) TxMsg());

  int int_temp;
  std::string temp;
  std::vector<SecretKey> secret_keys;

  msg.msg_type = TxMsg::MsgID;
  msg.msg_ts = std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::system_clock::now().time_since_epoch())
                   .count();

  log::info("How many input tx?");
  std::cin >> int_temp;
  msg.input_cnt = int_temp;

  for (int idx = 0; idx < msg.input_cnt; ++idx) {
    log::info("Input #{} id hex:", idx);
    std::cin >> temp;
    msg.get_input(idx).id = Ident::from_hex(temp);
    log::info("Input #{} sk hex:", idx);
    std::cin >> temp;
    secret_keys.push_back(SecretKey::from_hex(temp));
    msg.get_input(idx).vk = ed25519_sk_to_vk(secret_keys[idx]);
  }

  log::info("How many output tx?");
  std::cin >> int_temp;
  msg.output_cnt = int_temp;

  for (int idx = 0; idx < msg.output_cnt; ++idx) {
    log::info("Output #{} addr hex:", idx);
    std::cin >> temp;
    msg.get_output(idx).addr = Address::from_hex(temp);
    log::info("Output #{} value:", idx);
    std::cin >> msg.get_output(idx).value;
  }

  Hash tx_hash = msg_unique_hash(msg);
  for (int idx = 0; idx < msg.input_cnt; ++idx) {
    msg.get_input(idx).sig =
        ed25519_sign(secret_keys[idx], tx_hash.data(), Hash::Size);
  }

  log::info("{}", msg_as_hex(msg));
  return 0;
}
