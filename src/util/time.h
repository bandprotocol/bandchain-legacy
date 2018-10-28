// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the LICENSE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#pragma once

#include <enum/enum.h>

#include "inc/essential.h"
#include "util/buffer.h"

ENUM(TimeUnit, uint8_t, Second = 1, Minute = 2, Hour = 3, Day = 4, Week = 5,
     Month = 6, Year = 7)

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
