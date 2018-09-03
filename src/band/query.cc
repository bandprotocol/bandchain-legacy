#include "query.h"

#include "band/txgen.h"
#include "store/account.h"

Query::Query(Context& _ctx)
    : ctx(_ctx)
{
  dispatcher["balance"] = &Query::process_balance;
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
