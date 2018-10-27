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

#include <memory>
#include <utility>

class logger
{
public:
  static std::shared_ptr<spdlog::logger> get(std::string ident)
  {
    ident.insert(ident.begin(), (11 - ident.size()) / 2, ' ');
    ident.insert(ident.end(), 10 - ident.size(), ' ');

    auto log = spdlog::get(ident);
    if (log) {
      return log;
    } else {
      return spdlog::stdout_color_mt(ident);
    }
  }

  inline static std::shared_ptr<spdlog::logger> default_logger =
      spdlog::stdout_color_mt("band");

  inline static std::shared_ptr<spdlog::logger> nocommit_logger =
      spdlog::stdout_color_mt("NOCOMMIT");
};

#define DEBUG(log, ...) log->debug(__VA_ARGS__)
#define INFO(log, ...) log->info(__VA_ARGS__)
#define WARN(log, ...) log->warn(__VA_ARGS__)

#define LOG(...) logger::default_logger->info(__VA_ARGS__)
#define NOCOMMIT_LOG(...) logger::nocommit_logger->critical(__VA_ARGS__)
