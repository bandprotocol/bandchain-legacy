#pragma once

#include "inc/essential.h"
#include "store/context.h"
#include "util/json.h"

class Query
{
public:
  Query(Context& _ctx);

  /// Process the query. Return JSON encoded response.
  std::string process_query(const std::string& raw_data);

private:
  json process_balance(const json& params);
  json process_community_info(const json& params);

private:
  /// Reference to the primary blockchain context.
  Context& ctx;

  /// Dispatch from method name to the corresponding processing function.
  std::unordered_map<std::string, std::function<json(Query&, const json&)>>
      dispatcher;

  static inline auto log = logger::get("query");
};
