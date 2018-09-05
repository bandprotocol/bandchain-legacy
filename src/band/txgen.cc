#include "txgen.h"

#include <chrono>

#include "band/msg.h"
#include "util/buffer.h"
#include "util/bytes.h"
#include "util/endian.h"

using namespace std::chrono;

namespace
{
uint64_t get_current_ts()
{
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch())
      .count();
}
} // namespace

std::string txgen::process_txgen(const json& params)
{
  MsgHdr msg_hdr;

  msg_hdr.msgid = params.at("msgid");
  msg_hdr.ts = get_current_ts();
  msg_hdr.vk = VerifyKey::from_hex(params.at("vk").get<std::string>());

  std::string body;
  switch (msg_hdr.msgid) {
    case MintMsg::ID:
      body = process_mint(params);
      break;
    case TxMsg::ID:
      body = process_tx(params);
      break;
    case CreateMsg::ID:
      body = process_create(params);
      break;
    default:
      break;
  }

  // TODO add Signature
  return Buffer::serialize(msg_hdr) + body;
}

std::string txgen::process_mint(const json& params)
{
  MintMsg mint_msg;
  mint_msg.value = uint256_t(params.at("value").get<std::string>());
  return Buffer::serialize(mint_msg);
}

std::string txgen::process_tx(const json& params)
{
  TxMsg tx_msg;
  tx_msg.dest = Address::from_hex(params.at("dest").get<std::string>());
  tx_msg.value = uint256_t(params.at("value").get<std::string>());
  return Buffer::serialize(tx_msg);
}

std::string txgen::process_create(const json& params)
{
  CreateMsg create_msg;
  std::vector<std::string> op_codes = params.at("expressions");

  Buffer buf;
  for (const auto& op : op_codes) {
    if (op == "ADD")
      buf << OpCode::Add;
    else if (op == "SUB")
      buf << OpCode::Sub;
    else if (op == "MUL")
      buf << OpCode::Mul;
    else if (op == "DIV")
      buf << OpCode::Div;
    else if (op == "MOD")
      buf << OpCode::Mod;
    else if (op == "EXP")
      buf << OpCode::Exp;
    else if (op == "X") {
      buf << OpCode::Variable;
      buf << Variable::Supply;
    } else {
      uint256_t num = uint256_t(op);
      buf << OpCode::Constant;
      buf << num;
    }
  }
  buf >> create_msg.curve;

  return Buffer::serialize(create_msg);
}
