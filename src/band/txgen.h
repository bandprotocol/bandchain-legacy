#pragma once

#include "inc/essential.h"
#include "util/json.h"

namespace txgen
{
/// Generate transaction based on the given spec (without signature).
json process_txgen(const json& params);

/// One function per one message type.
std::string process_mint(const json& params);
std::string process_tx(const json& params);
std::string process_create_contract(const json& params);
std::string process_purchase_contract(const json& params);
std::string process_sell_contract(const json& params);

} // namespace txgen
