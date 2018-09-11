#include <chrono>
#include <iostream>

#include "band/msg.h"
#include "crypto/ed25519.h"

using namespace std::chrono;

template <typename T>
T read(const char* input_msg)
{
  LOG(input_msg);
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
  LOG("MESSAGE: {}", buf);
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
  return 0;
}
