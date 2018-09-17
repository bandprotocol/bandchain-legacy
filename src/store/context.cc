#include "context.h"

void Context::reset()
{
  // No more cache...
  cache.clear();
}

void Context::flush()
{
  for (auto& obj : cache) {
    add_impl(std::move(obj.second));
  }
}

// Object* Context::get_context_impl(const ContextKey& key)
// {
//   auto it = cache.find(key);
//   if (it != cache.end())
//     return it->second.get();

//   // X
// }
// auto uniq = std::make_unique<T>(key, std::forward<Args>(args)...);
// auto raw = uniq.get();
// add(std::move(uniq));
// return *raw;
