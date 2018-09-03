#include <chrono>
#include <iostream>

#include "band/msg.h"
#include "crypto/ed25519.h"

using namespace std::chrono;

template <typename T>
T read(const char* input_msg)
{
  log::info(input_msg);
  T read_buffer;
  std::cin >> read_buffer;
  return read_buffer;
}

template <typename T>
T read_hex(const char* input_msg)
{
  auto read_buffer = read<std::string>(input_msg);
  return T::from_hex(read_buffer);
}

template <typename T>
void finalize(const MsgHdr& msg_hdr, const T& msg_body, const SecretKey& sk)
{
  Buffer buf;
  buf << msg_hdr;
  buf << msg_body;
  buf << ed25519_sign(sk, buf.as_span());
  log::info("MESSAGE: {}", buf);
}

int main()
{
  auto sk = read_hex<SecretKey>("What's your secret key?");

  MsgHdr msg_hdr;
  msg_hdr.msgid = read<uint16_t>("What's the message ID?");
  msg_hdr.ts =
      duration_cast<milliseconds>(system_clock::now().time_since_epoch())
          .count();
  msg_hdr.vk = ed25519_sk_to_vk(sk);

  switch (msg_hdr.msgid) {
    case MintMsg::ID: {
      MintMsg mint_msg;
      mint_msg.token_key = read_hex<TokenKey>("What's the token key?");
      mint_msg.value = read<uint256_t>("How much to mint?");
      finalize(msg_hdr, mint_msg, sk);
      break;
    }
    case TxMsg::ID: {
      TxMsg tx_msg;
      tx_msg.dest = read_hex<Address>("What's the destination address?");
      tx_msg.token_key = read_hex<TokenKey>("What's the token key?");
      tx_msg.value = read<uint256_t>("How much to send?");
      finalize(msg_hdr, tx_msg, sk);
      break;
    }
    default:
      break;
  }

  // MintMsg msg;
  // log::info("{}", msg.size());
  // log::info("{}", msg);

  // char buffer[1 << 16];

  // int int_temp;
  // uint256_t uint256_temp;
  // std::string temp;
  // std::vector<SecretKey> secret_keys;

  // log::info("Is it a mint tx? 0 or 1");
  // std::cin >> int_temp;

  // if (int_temp == 1) {
  //   MintMsg& msg = *(new (buffer) MintMsg());
  //   msg.msg_type = MintMsg::MsgID;

  //   log::info("Mint address");
  //   std::cin >> temp;
  //   msg.addr = Address::from_hex(temp);

  //   log::info("Mint value");
  //   std::cin >> uint256_temp;
  //   msg.value = BigInt::from_uint256(uint256_temp);

  //   log::info("Mint ident");
  //   std::cin >> temp;
  //   msg.ident = Hash::from_hex(temp);

  //   log::info("{}", msg_hex(msg));
  //   return 0;
  // }

  // TxMsg& msg = *(new (buffer) TxMsg());
  // msg.msg_type = TxMsg::MsgID;
  // msg.msg_ts = std::chrono::duration_cast<std::chrono::milliseconds>(
  //                  std::chrono::system_clock::now().time_since_epoch())
  //                  .count();

  // log::info("How many input tx?");
  // std::cin >> int_temp;
  // msg.input_cnt = int_temp;

  // for (int idx = 0; idx < msg.input_cnt; ++idx) {
  //   log::info("Input #{} id hex:", idx);
  //   std::cin >> temp;
  //   msg.get_input(idx).ident = Hash::from_hex(temp);
  //   log::info("Input #{} sk hex:", idx);
  //   std::cin >> temp;
  //   secret_keys.push_back(SecretKey::from_hex(temp));
  //   msg.get_input(idx).vk = ed25519_sk_to_vk(secret_keys[idx]);
  // }

  // log::info("How many output tx?");
  // std::cin >> int_temp;
  // msg.output_cnt = int_temp;

  // for (int idx = 0; idx < msg.output_cnt; ++idx) {
  //   log::info("Output #{} addr hex:", idx);
  //   std::cin >> temp;
  //   msg.get_output(idx).addr = Address::from_hex(temp);
  //   log::info("Output #{} value:", idx);
  //   std::cin >> uint256_temp;
  //   msg.get_output(idx).value = BigInt::from_uint256(uint256_temp);
  // }

  // Hash tx_hash = msg.hash();
  // for (int idx = 0; idx < msg.input_cnt; ++idx) {
  //   msg.get_signature(idx) =
  //       ed25519_sign(secret_keys[idx], tx_hash.data(), Hash::Size);
  // }

  // log::info("{}", msg_hex(msg));
  return 0;
}
