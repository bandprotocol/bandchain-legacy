#include "query.h"

#include <boost/scope_exit.hpp>

#include "band/txgen.h"
#include "store/account.h"
#include "store/contract.h"
#include "util/iban.h"

namespace
{
void log_debug_json_pretty(std::shared_ptr<spdlog::logger> log,
                           const json& data, const std::string& indent)
{
  std::stringstream stream(data.dump(2));
  while (!stream.eof()) {
    std::string line;
    std::getline(stream, line);
    DEBUG(log, "{}{}", indent, line);
  }
}
void log_debug_query(std::shared_ptr<spdlog::logger> log,
                     const std::string& method, const json& req,
                     const json& res)
{
  DEBUG(log, "QUERY {}", method);
  DEBUG(log, "  Request:");
  log_debug_json_pretty(log, req, "    ");
  DEBUG(log, "  Response:");
  log_debug_json_pretty(log, res, "    ");
  DEBUG(log, "============================================================"
             "============================================================");
}
} // namespace

Query::Query(Context& _ctx)
    : ctx(_ctx)
{
  dispatcher["balance"] = &Query::process_balance;
  dispatcher["community_info"] = &Query::process_community_info;
}

std::string Query::process_query(const std::string& raw_data)
{
  BOOST_SCOPE_EXIT(&ctx) { ctx.reset(); }
  BOOST_SCOPE_EXIT_END

  auto data = json::parse(raw_data);
  if (!data.is_object())
    throw Error("Invalid query data. Must be JSON object.");

  const auto& method = data.at("method").get<std::string>();
  const auto& params = data.at("params");

  if (method == "txgen") {
    auto res = txgen::process_txgen(params);
    log_debug_query(log, method, params, res);
    return res.dump();
  }

  if (auto it = dispatcher.find(method); it != dispatcher.end()) {
    auto res = it->second(*this, params);
    log_debug_query(log, method, params, res);
    return res.dump();
  } else {
    throw Error("Unknown query method");
  }
}

json Query::process_balance(const json& params)
{
  const auto& address_iban = params.at("address").get<std::string>();
  const auto& token_iban = params.at("token").get<std::string>();

  auto address = IBAN(address_iban, IBANType::Account).as_addr();
  auto token = IBAN(token_iban, IBANType::Contract).as_addr();

  auto& account = ctx.get<Account>(address);
  const uint256_t balance = account[token];

  json response;
  response["balance"] = "{}"_format(balance);
  return response;
}

json Query::process_community_info(const json& params)
{
  const auto& contract_id_iban = params.at("contract_id").get<std::string>();

  auto contract_id = IBAN(contract_id_iban, IBANType::Contract).as_addr();

  auto& contract = ctx.get<Contract>(contract_id);
  PriceSpread ps = contract.get_price_spread();

  json response;
  response["equation"] = contract.get_string_equation();
  response["circulating_supply"] = "{}"_format(contract.circulating_supply);
  response["total_supply"] = "{}"_format(contract.total_supply);
  response["max_supply"] = "{}"_format(contract.max_supply);
  response["spread_type"] = ps.get_spread_type()._to_string();
  response["spread_value"] = "{}"_format(ps.get_spread_value());

  response["revenue_id"] =
      contract.revenue_id.to_iban_string(IBANType::Revenue);

  response["is_transferable"] = contract.is_transferable;
  response["is_discountable"] = contract.is_discountable;
  return response;
}
