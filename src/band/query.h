#pragma once

#include "store/context.h"

class Query
{
public:
  Query(Context& _ctx);

  /// Process the query. Return JSON encoded response.
  std::string process_query(const std::string& raw_data);

private:
  Context& ctx;
};
