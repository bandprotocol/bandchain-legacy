#include "query.h"

#include "util/json.h"

Query::Query(Context& _ctx)
    : ctx(_ctx)
{
}

std::string Query::process_query(const std::string& raw_data)
{
  try {
    auto data = json::parse(raw_data);
    return data.dump();
  } catch (nlohmann::detail::parse_error&) {
    throw Error("JSON parse error");
  }
}
