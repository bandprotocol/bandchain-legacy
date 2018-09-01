#pragma once

#include <memory>
#include <utility>

class log
{
public:
  template <typename... Args>
  static void debug(Args&&... args)
  {
    logger->debug(std::forward<Args>(args)...);
  }

  template <typename... Args>
  static void info(Args&&... args)
  {
    logger->info(std::forward<Args>(args)...);
  }

  template <typename... Args>
  static void warn(Args&&... args)
  {
    logger->warn(std::forward<Args>(args)...);
  }

  template <typename... Args>
  static void error(Args&&... args)
  {
    logger->error(std::forward<Args>(args)...);
  }

private:
  inline static std::shared_ptr<spdlog::logger> logger =
      spdlog::stdout_color_mt("band");
};
