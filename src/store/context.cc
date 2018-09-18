#include "context.h"

void Context::reset()
{
  // No more cache...
  cache.clear();
}

void Context::flush()
{
  for (auto& obj : cache) {
    obj.second->debug_save();
    add_impl(std::move(obj.second));
  }
  cache.clear();

  DEBUG(log, "============================================================"
             "============================================================");
}
