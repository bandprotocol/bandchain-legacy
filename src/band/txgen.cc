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
void parse_equation(Buffer& buf, const std::vector<std::string>& op_codes)
{
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
    else if (op == "Supply") {
      buf << OpCode::Variable;
    } else if (op.size() >= 36 and op.substr(0, 2) == "CX") {
      buf << OpCode::Contract;
      buf << IBAN(op, IBANType::Contract).as_addr();
    } else if (op.size() >= 36 and op.substr(0, 2) == "PX") {
      buf << OpCode::Price;
      buf << IBAN(op, IBANType::Price).as_addr();
    } else {
      uint256_t num = uint256_t(op);
      buf << OpCode::Constant;
      buf << num;
    }
  }
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
    case CreateContractMsg::ID:
      body = process_create_contract(params);
      break;
    case PurchaseContractMsg::ID:
      body = process_purchase_contract(params);
      break;
    case SellContractMsg::ID:
      body = process_sell_contract(params);
      break;
    // case CreatePCMsg::ID:
    //   body = process_createPC(params);
    //   break;
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
      IBAN(params.at("token").get<std::string>(), IBANType::Contract).as_addr();
  mint_msg.value = uint256_t(params.at("value").get<std::string>());
  return Buffer::serialize(mint_msg);
}

std::string txgen::process_tx(const json& params)
{
  TxMsg tx_msg;
  tx_msg.token_key =
      IBAN(params.at("token").get<std::string>(), IBANType::Contract).as_addr();
  tx_msg.dest =
      IBAN(params.at("dest").get<std::string>(), IBANType::Account).as_addr();
  tx_msg.value = uint256_t(params.at("value").get<std::string>());
  return Buffer::serialize(tx_msg);
}

std::string txgen::process_create_contract(const json& params)
{
  CreateContractMsg create_msg;

  create_msg.revenue_id =
      IBAN(params.at("revenue_id").get<std::string>(), IBANType::Revenue)
          .as_addr();

  std::vector<std::string> op_codes = params.at("expressions");
  Buffer buf;
  parse_equation(buf, op_codes);

  SpreadType t = SpreadType(
      uint8_t(std::stoul(params.at("spread_type").get<std::string>())));
  uint256_t value = uint256_t(params.at("spread_value").get<std::string>());

  PriceSpread price_spread(t, value);
  buf << price_spread;
  buf >> create_msg.curve;

  create_msg.max_supply = uint256_t(params.at("max_supply").get<std::string>());
  create_msg.is_transferable =
      uint8_t(std::stoul(params.at("is_transferable").get<std::string>()));
  create_msg.is_discountable =
      uint8_t(std::stoul(params.at("is_discountable").get<std::string>()));
  create_msg.beneficiary =
      IBAN(params.at("beneficiary").get<std::string>(), IBANType::Account)
          .as_addr();
  return Buffer::serialize(create_msg);
}

std::string txgen::process_purchase_contract(const json& params)
{
  PurchaseContractMsg pct_msg;
  pct_msg.value = uint256_t(params.at("value").get<std::string>());
  pct_msg.price_limit = uint256_t(params.at("price_limit").get<std::string>());
  pct_msg.contract_id =
      IBAN(params.at("contract_id").get<std::string>(), IBANType::Contract)
          .as_addr();

  return Buffer::serialize(pct_msg);
}

std::string txgen::process_sell_contract(const json& params)
{
  SellContractMsg sct_msg;
  sct_msg.value = uint256_t(params.at("value").get<std::string>());
  sct_msg.price_limit = uint256_t(params.at("price_limit").get<std::string>());
  sct_msg.contract_id =
      IBAN(params.at("contract_id").get<std::string>(), IBANType::Contract)
          .as_addr();

  return Buffer::serialize(sct_msg);
}
