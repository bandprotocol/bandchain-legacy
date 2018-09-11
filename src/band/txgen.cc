#include "txgen.h"

#include <chrono>

#include "band/msg.h"
#include "crypto/ed25519.h"
#include "util/buffer.h"
#include "util/bytes.h"
#include "util/endian.h"
#include "util/equation.h"
#include "util/iban.h"

using namespace std::chrono;

namespace
{
uint64_t get_current_ts()
{
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch())
      .count();
}

} // namespace

json txgen::process_txgen(const json& params)
{
  MsgHdr msg_hdr;

  msg_hdr.msgid = std::stoul(params.at("msgid").get<std::string>());
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
    case PurchaseCTMsg::ID:
      body = process_purchaseCT(params);
      break;
    case SellCTMsg::ID:
      body = process_sellCT(params);
      break;
    default:
      throw Error("Message ID doesn't match any ID in system");
  }

  std::string msg_bin = Buffer::serialize(msg_hdr) + body;
  if (params.count("sk") == 1) {
    SecretKey sk = SecretKey::from_hex(params.at("sk").get<std::string>());
    Signature sig = ed25519_sign(sk, gsl::make_span(msg_bin));
    msg_bin += Buffer::serialize(sig);
  }

  json response;
  response["tx"] = bytes_to_hex(gsl::make_span(msg_bin));
  return response;
}

std::string txgen::process_mint(const json& params)
{
  MintMsg mint_msg;
  mint_msg.token_key =
      IBAN(params.at("token").get<std::string>(), IBANType::Token).as_addr();
  mint_msg.value = uint256_t(params.at("value").get<std::string>());
  return Buffer::serialize(mint_msg);
}

std::string txgen::process_tx(const json& params)
{
  TxMsg tx_msg;
  tx_msg.token_key =
      IBAN(params.at("token").get<std::string>(), IBANType::Token).as_addr();
  tx_msg.dest =
      IBAN(params.at("dest").get<std::string>(), IBANType::Account).as_addr();
  tx_msg.value = uint256_t(params.at("value").get<std::string>());
  return Buffer::serialize(tx_msg);
}

std::string txgen::process_create(const json& params)
{
  CreateMsg create_msg;
  create_msg.max_supply = uint256_t(params.at("max_supply").get<std::string>());
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

  SpreadType t = SpreadType(
      uint8_t(std::stoul(params.at("spread_type").get<std::string>())));
  uint256_t value = uint256_t(params.at("spread_value").get<std::string>());

  PriceSpread price_spread(t, value);
  buf << price_spread;
  buf >> create_msg.curve;

  return Buffer::serialize(create_msg);
}

std::string txgen::process_purchaseCT(const json& params)
{
  PurchaseCTMsg pct_msg;
  pct_msg.value = uint256_t(params.at("value").get<std::string>());
  pct_msg.band_limit = uint256_t(params.at("band_limit").get<std::string>());
  pct_msg.contract_id =
      IBAN(params.at("contract_id").get<std::string>(), IBANType::Token)
          .as_addr();

  return Buffer::serialize(pct_msg);
}

std::string txgen::process_sellCT(const json& params)
{
  SellCTMsg sct_msg;
  sct_msg.value = uint256_t(params.at("value").get<std::string>());
  sct_msg.band_limit = uint256_t(params.at("band_limit").get<std::string>());
  sct_msg.contract_id =
      IBAN(params.at("contract_id").get<std::string>(), IBANType::Token)
          .as_addr();

  return Buffer::serialize(sct_msg);
}
