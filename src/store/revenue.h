#pragma once

#include <unordered_map>

#include "inc/essential.h"
#include "store/context.h"
#include "util/equation.h"
#include "util/time.h"

class Revenue final : public ObjectImpl<IBANType::Revenue>
{
public:
  Revenue(const Revenue& revenue) = default;

  Revenue(const RevenueID& revenue_id, const ContractID& _base_token_id,
          const AccountID& _manager, const TimePeriod& _time_period,
          const StakeID& _stake_id, bool _is_private,
          const Curve& _curve_to_base, const Curve& _curve_to_x);

  uint64_t get_tpc(int64_t timestamp) const;

  uint256_t& operator[](uint64_t tpc);

  uint256_t base_to_x(const uint256_t& base) const;

  uint256_t x_to_base(const uint256_t& x) const;

  void debug_create() const final;

  void debug_save() const final;

public:
  const ContractID base_token_id;
  const AccountID manager;
  const TimePeriod time_period;
  const StakeID stake_id;
  const bool is_private;
  const Curve curve_to_base;
  const Curve curve_to_x;

private:
  std::unordered_map<uint64_t, uint256_t> revenues;

  static inline auto log = logger::get("revenue");
};
