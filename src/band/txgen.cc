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
      buf << ContractID::from_string(op);
    } else if (op.size() >= 36 and op.substr(0, 2) == "PX") {
      buf << OpCode::Price;
      buf << PriceID::from_string(op);
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

  msg_hdr.msgid =
      MsgID::_from_string(params.at("msgid").get<std::string>().c_str());
  msg_hdr.ts = get_current_ts();
  msg_hdr.vk = VerifyKey::from_hex(params.at("vk").get<std::string>());

  std::string body;
  switch (msg_hdr.msgid) {
    case +MsgID::Mint:
      body = process_mint(params);
      break;
    case +MsgID::Tx:
      body = process_tx(params);
      break;
    case +MsgID::CreateContract:
      body = process_create_contract(params);
      break;
    case +MsgID::PurchaseContract:
      body = process_purchase_contract(params);
      break;
    case +MsgID::SellContract:
      body = process_sell_contract(params);
      break;
    case +MsgID::SpendToken:
      body = process_spend_token(params);
      break;
    case +MsgID::CreateRevenue:
      body = process_create_revenue(params);
      break;
    case +MsgID::Unset:
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
      ContractID::from_string(params.at("token").get<std::string>());
  mint_msg.value = uint256_t(params.at("value").get<std::string>());
  return Buffer::serialize(mint_msg);
}

std::string txgen::process_tx(const json& params)
{
  TxMsg tx_msg;
  tx_msg.token_key =
      ContractID::from_string(params.at("token").get<std::string>());
  tx_msg.dest = AccountID::from_string(params.at("dest").get<std::string>());
  tx_msg.value = uint256_t(params.at("value").get<std::string>());
  return Buffer::serialize(tx_msg);
}

std::string txgen::process_create_contract(const json& params)
{
  CreateContractMsg create_msg;

  create_msg.revenue_id =
      RevenueID::from_string(params.at("revenue_id").get<std::string>());

  std::vector<std::string> op_codes = params.at("buy_expressions");
  Buffer buf;
  parse_equation(buf, op_codes);
  buf >> create_msg.buy_curve;

  op_codes = params.at("sell_expressions").get<std::vector<std::string>>();
  parse_equation(buf, op_codes);
  buf >> create_msg.sell_curve;

  create_msg.max_supply = uint256_t(params.at("max_supply").get<std::string>());
  create_msg.is_transferable =
      uint8_t(std::stoul(params.at("is_transferable").get<std::string>()));
  create_msg.is_discountable =
      uint8_t(std::stoul(params.at("is_discountable").get<std::string>()));
  create_msg.beneficiary =
      AccountID::from_string(params.at("beneficiary").get<std::string>());
  return Buffer::serialize(create_msg);
}

std::string txgen::process_purchase_contract(const json& params)
{
  PurchaseContractMsg pct_msg;
  pct_msg.value = uint256_t(params.at("value").get<std::string>());
  pct_msg.price_limit = uint256_t(params.at("price_limit").get<std::string>());
  pct_msg.contract_id =
      ContractID::from_string(params.at("contract_id").get<std::string>());

  return Buffer::serialize(pct_msg);
}

std::string txgen::process_sell_contract(const json& params)
{
  SellContractMsg sct_msg;
  sct_msg.value = uint256_t(params.at("value").get<std::string>());
  sct_msg.price_limit = uint256_t(params.at("price_limit").get<std::string>());
  sct_msg.contract_id =
      ContractID::from_string(params.at("contract_id").get<std::string>());

  return Buffer::serialize(sct_msg);
}

std::string txgen::process_spend_token(const json& params)
{
  SpendTokenMsg spend_msg;
  spend_msg.token_key =
      ContractID::from_string(params.at("token_id").get<std::string>());
  spend_msg.value = uint256_t(params.at("value").get<std::string>());

  return Buffer::serialize(spend_msg);
}

std::string txgen::process_create_revenue(const json& params)
{
  CreateRevenueMsg cr_msg;
  cr_msg.base_token_id =
      ContractID::from_string(params.at("base_token_id").get<std::string>());
  cr_msg.stake_id =
      StakeID::from_string(params.at("stake_id").get<std::string>());

  TimeUnit time_unit =
      TimeUnit::_from_string(params.at("time_unit").get<std::string>().c_str());
  uint16_t time_value =
      uint16_t(std::stoul(params.at("time_value").get<std::string>()));
  cr_msg.time_period = TimePeriod(time_value, time_unit);
  cr_msg.is_private =
      bool(std::stoul(params.at("is_private").get<std::string>()));

  return Buffer::serialize(cr_msg);
}
