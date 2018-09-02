#pragma once

#include "store/context.h"
#include "util/json.h"

class Query
{
public:
  Query(Context& _ctx);

  /// Process the query. Return JSON encoded response.
  std::string process_query(const std::string& raw_data);

private:
  std::string process_balance(const json& params);

private:
  /// Reference to the primary blockchain context.
  Context& ctx;

  /// Dispatch from method name to the corresponding processing function.
  std::unordered_map<std::string,
                     std::function<std::string(Query&, const json&)>>
      dispatcher;
};
