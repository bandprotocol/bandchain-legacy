#pragma once

#include <exception>

class Error : public std::logic_error
{
public:
  template <typename... Args>
  explicit Error(Args&&... args)
      : std::logic_error(fmt::format(std::forward<Args>(args)...))
  {
  }
};

class Failure : public std::runtime_error
{
public:
  template <typename... Args>
  explicit Failure(Args&&... args)
      : std::runtime_error(fmt::format(std::forward<Args>(args)...))
  {
  }
};
