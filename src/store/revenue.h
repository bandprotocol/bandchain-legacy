#pragma once

#include <unordered_map>

#include "inc/essential.h"
#include "store/context.h"
#include "util/time.h"

class Revenue : public Object
{
public:
  Revenue(const ContextKey& revenue_id, const TokenKey& _base_token_id,
          const Address& _manager, const TimePeriod& _time_period,
          const ContextKey& _stake_id, bool _is_private);

  Revenue(const Revenue& revenue);
  // Getter
  TokenKey get_base_token_id() const;

  Address get_manager() const;

  ContextKey get_stake_id() const;

  bool get_is_private() const;

  uint64_t get_tpc(int64_t timestamp) const;

  uint256_t get_period_revenue(uint64_t period_number) const;

  void set_period_revenue(uint64_t period_number, const uint256_t& value);

  uint256_t token_to_x(const uint256_t& tokens) const;

  uint256_t x_to_token(const uint256_t& x) const;

  void debug_create() const final;

  void debug_save() const final;

private:
  TokenKey base_token_id;
  Address manager;
  TimePeriod time_period;
  ContextKey stake_id;
  bool is_private;
  std::unordered_map<uint64_t, uint256_t> revenues;

  static inline auto log = logger::get("store/revenue");
};
