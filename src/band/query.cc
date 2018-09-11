#include "query.h"

#include "band/txgen.h"
#include "store/account.h"
#include "store/ccontract.h"
#include "util/iban.h"

Query::Query(Context& _ctx)
    : ctx(_ctx)
{
  dispatcher["balance"] = &Query::process_balance;
  dispatcher["community_info"] = &Query::process_community_info;
}

std::string Query::process_query(const std::string& raw_data)
{
  auto data = json::parse(raw_data);
  if (!data.is_object())
    throw Error("Invalid query data. Must be JSON object.");

  const auto& method = data.at("method").get<std::string>();
  const auto& params = data.at("params");

  if (method == "txgen") {
    return txgen::process_txgen(params).dump();
  }

  if (auto it = dispatcher.find(method); it != dispatcher.end()) {
    return it->second(*this, params).dump();
  } else {
    throw Error("Unknown query method");
  }
}

json Query::process_balance(const json& params)
{
  const auto& address_iban = params.at("address").get<std::string>();
  const auto& token_iban = params.at("token").get<std::string>();

  auto address = IBAN(address_iban, IBANType::Account).as_addr();
  auto token = IBAN(token_iban, IBANType::Token).as_addr();

  Account account(ctx, address);
  const uint256_t balance = account.get_balance(token);

  log::debug("addr: {}", IBAN(address, IBANType::Account).as_addr());
  log::debug("token: {}", IBAN(token, IBANType::Token).as_addr());
  log::debug("balance: {}", balance);

  json response;
  response["balance"] = "{}"_format(balance);
  return response;
}

json Query::process_community_info(const json& params)
{
  const auto& contract_id_iban = params.at("contract_id").get<std::string>();

  auto contract_id = IBAN(contract_id_iban, IBANType::Token).as_addr();

  CommunityContract contract(ctx, contract_id);
  const std::string equation = contract.get_string_equation();
  const uint256_t current_supply = contract.get_current_supply();
  const uint256_t max_supply = contract.get_max_supply();
  PriceSpread ps = contract.get_price_spread();
  const uint256_t current_profit = contract.get_current_profit();

  json response;
  response["equation"] = equation;
  response["current_supply"] = "{}"_format(current_supply);
  response["max_supply"] = "{}"_format(max_supply);
  response["spread_type"] = "{}"_format((uint8_t)ps.get_spread_type());
  response["spread_value"] = "{}"_format(ps.get_spread_value());
  response["current_profit"] = "{}"_format(current_profit);
  return response;
}