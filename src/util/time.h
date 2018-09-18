#pragma once

#include <enum/enum.h>

#include "inc/essential.h"
#include "util/buffer.h"

BETTER_ENUM(TimeUnit, uint8_t, Second = 1, Minute = 2, Hour = 3, Day = 4,
            Week = 5, Month = 6, Year = 7)

class TimePeriod
{
public:
  TimePeriod();

  TimePeriod(const TimePeriod& time_period) = default;

  TimePeriod(uint16_t _time_value, TimeUnit _time_unit);

  TimePeriod& operator=(const TimePeriod& rhs) = default;

  /// Convert the given timestamp into the time period count
  uint64_t timestamp_to_tpc(int64_t timestamp) const;

  friend Buffer& operator>>(Buffer& buf, TimePeriod& period);
  friend Buffer& operator<<(Buffer& buf, const TimePeriod& period);

  uint16_t time_value;
  TimeUnit time_unit;
};
