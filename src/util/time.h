#include <enum/enum.h>

#include "inc/essential.h"
#include "util/buffer.h"

BETTER_ENUM(TimeUnit, uint8_t, Second, Minute, Hour, Day, Week, Month, Year)

class TimePeriod
{
public:
  TimePeriod();
  TimePeriod(uint16_t _time_value, TimeUnit _time_unit);

  /// Convert the given timestamp into the time period count
  uint64_t timestamp_to_tpc(int64_t timestamp);

  friend Buffer& operator>>(Buffer& buf, TimePeriod& period);
  friend Buffer& operator<<(Buffer& buf, const TimePeriod& period);

private:
  uint16_t time_value;
  TimeUnit time_unit;
};
