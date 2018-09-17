#include <cxxtest/TestSuite.h>

#include "inc/essential.h"
#include "util/time.h"

class TimeTest : public CxxTest::TestSuite
{
public:
  void testDifferentTimePeriod()
  {
    auto period = TimePeriod(5, TimeUnit::Minute);
    auto ts1 = get_ts_from_str("2018-01-01T22:00:01Z");
    auto ts2 = get_ts_from_str("2018-01-01T22:05:00Z");
    auto ts3 = get_ts_from_str("2018-01-02T22:01:02Z");

    TS_ASSERT_DIFFERS(period.timestamp_to_tpc(ts1),
                      period.timestamp_to_tpc(ts2));
    TS_ASSERT_DIFFERS(period.timestamp_to_tpc(ts1),
                      period.timestamp_to_tpc(ts3));
    TS_ASSERT_DIFFERS(period.timestamp_to_tpc(ts2),
                      period.timestamp_to_tpc(ts3));
  }

  void testSameTimePeriod()
  {
    auto period = TimePeriod(5, TimeUnit::Minute);
    auto ts1 = get_ts_from_str("2018-01-01T22:00:01Z");
    auto ts2 = get_ts_from_str("2018-01-01T22:01:20Z");
    auto ts3 = get_ts_from_str("2018-01-01T22:04:59Z");

    TS_ASSERT_EQUALS(period.timestamp_to_tpc(ts1),
                     period.timestamp_to_tpc(ts2));
    TS_ASSERT_EQUALS(period.timestamp_to_tpc(ts1),
                     period.timestamp_to_tpc(ts3));
  }

  void testMonthTimePeriod()
  {
    auto period = TimePeriod(1, TimeUnit::Month);
    auto ts1 = get_ts_from_str("2018-01-01T13:10:32Z");
    auto ts2 = get_ts_from_str("2018-01-15T20:32:45Z");
    auto ts3 = get_ts_from_str("2018-02-01T11:05:59Z");

    TS_ASSERT_EQUALS(period.timestamp_to_tpc(ts1),
                     period.timestamp_to_tpc(ts2));
    TS_ASSERT_DIFFERS(period.timestamp_to_tpc(ts1),
                      period.timestamp_to_tpc(ts3));
  }

private:
  int64_t get_ts_from_str(const char* str)
  {
    std::tm time{};
    std::istringstream stream(str);
    stream >> std::get_time(&time, "%Y-%m-%dT%H:%M:%S");
    return timegm(&time);
  }
};
