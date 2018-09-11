#pragma once

#include <memory>
#include <utility>

class logger
{
public:
  static std::shared_ptr<spdlog::logger> get(const std::string& ident)
  {
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
