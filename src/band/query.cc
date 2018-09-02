#include "query.h"

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

    auto method = data["method"];
    if (!method.is_string())
      throw Error("Invalid query method. Must be JSON string.");

    auto params = data["params"];
    if (!params.is_object())
      throw Error("Invalid query params. Must be JSON object");

    if (auto it = dispatcher.find(method); it != dispatcher.end()) {
      return it->second(*this, params);
    } else {
      throw Error("Unknown query method");
    }

  } catch (const nlohmann::detail::parse_error&) {
    throw Error("JSON parse error");
  }
}

std::string Query::process_balance(const json& params)
{
  // TODO
  return "TODO";
}
