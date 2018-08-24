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
