#include "time.h"

#include <ctime>

#include "util/endian.h"

namespace
{
uint64_t get_total_count(TimeUnit time_unit, int64_t timestamp)
{
  std::time_t timestamp_time_t = timestamp;
  auto time = std::gmtime(&timestamp_time_t);

  switch (time_unit) {
    case +TimeUnit::Second:
      return timestamp;
    case +TimeUnit::Minute:
      return timestamp / 60;
    case +TimeUnit::Hour:
      return timestamp / 3600;
    case +TimeUnit::Day:
      return timestamp / 86400;
    case +TimeUnit::Week:
      return (timestamp - 3 * 86400) / (7 * 86400);
    case +TimeUnit::Month:
      return 12 * time->tm_year + time->tm_mon;
    case +TimeUnit::Year:
      return time->tm_year;
  }
  throw Failure("Invalid time unit");
}
} // namespace

TimePeriod::TimePeriod()
    : TimePeriod(0, TimeUnit::Second)
{
}

TimePeriod::TimePeriod(uint16_t _time_value, TimeUnit _time_unit)
    : time_value(_time_value)
    , time_unit(_time_unit)
{
}

Buffer& operator<<(Buffer& buf, const TimePeriod& period)
{
  return buf << period.time_value << period.time_unit;
}

Buffer& operator>>(Buffer& buf, TimePeriod& period)
{
  return buf >> period.time_value >> period.time_unit;
}

uint64_t TimePeriod::timestamp_to_tpc(int64_t timestamp) const
{
  uint64_t total_count = get_total_count(time_unit, timestamp);
  return total_count / time_value;
}
