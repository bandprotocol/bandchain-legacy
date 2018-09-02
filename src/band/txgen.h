#pragma once

#include "util/json.h"

namespace txgen
{
/// Generate transaction based on the given spec (without signature).
std::string process_txgen(const json& params);

/// One function per one message type.
std::string process_mint(const json& params);
std::string process_tx(const json& params);

} // namespace txgen
