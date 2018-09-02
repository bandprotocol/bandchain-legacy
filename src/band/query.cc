#include "query.h"

#include "store/account.h"

Query::Query(Context& _ctx)
    : ctx(_ctx)
{
  dispatcher["balance"] = &Query::process_balance;
}

std::string Query::process_query(const std::string& raw_data)
{
  try {
    auto data = json::parse(raw_data);
    if (!data.is_object())
      throw Error("Invalid query data. Must be JSON object.");

    auto method = data.at("method").get<std::string>();
    auto params = data.at("params");

    if (auto it = dispatcher.find(method); it != dispatcher.end()) {
      return it->second(*this, params).dump();
    } else {
      throw Error("Unknown query method");
    }

  } catch (const json::parse_error& err) {
    throw Error("JSON parse error: {}", err.what());
  } catch (const json::out_of_range& err) {
    throw Error("JSON range error: {}", err.what());
  } catch (const json::type_error& err) {
    throw Error("JSON type error: {}", err.what());
  }
}

json Query::process_balance(const json& params)
{
  auto address = params.at("address").get<std::string>();
  auto token = params.at("token").get<std::string>();

  Account account(ctx, Address::from_hex(address));
  const uint256_t balance = account.get_balance(TokenKey::from_hex(token));

  json response;
  response["balance"] = "{}"_format(balance);
  return response;
}
