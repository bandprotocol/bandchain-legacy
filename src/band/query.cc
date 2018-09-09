#include "query.h"

#include "band/txgen.h"
#include "store/account.h"
#include "store/ccontract.h"

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
    return txgen::process_txgen(params);
  }

  if (auto it = dispatcher.find(method); it != dispatcher.end()) {
    return it->second(*this, params).dump();
  } else {
    throw Error("Unknown query method");
  }
}

json Query::process_balance(const json& params)
{
  const auto& address = params.at("address").get<std::string>();
  const auto& token = params.at("token").get<std::string>();

  Account account(ctx, Address::from_hex(address));
  const uint256_t balance = account.get_balance(TokenKey::from_hex(token));

  json response;
  response["balance"] = "{}"_format(balance);
  return response;
}

json Query::process_community_info(const json& params)
{
  const auto& contract_id = params.at("contract_id").get<std::string>();

  CommunityContract contract(ctx, ContractID::from_hex(contract_id));
  const std::string equation = contract.get_string_equation();
  const uint256_t current_supply = contract.get_current_supply();
  const uint256_t max_supply = contract.get_max_supply();

  json response;
  response["equation"] = equation;
  response["current_supply"] = "{}"_format(current_supply);
  response["max_supply"] = "{}"_format(max_supply);

  return response;
}
