#include "context_map.h"

ContextMap::ContextMap() { DEBUG(log, "CONTEXT_MAP_INIT"); }

Object* ContextMap::get_impl(const ContextKey& key) const
{
  if (auto it = data.find(key); it != data.end()) {
    return it->second.get();
  }
  return nullptr;
}

void ContextMap::add_impl(std::unique_ptr<Object> obj)
{
  data[obj->key] = std::move(obj);
}
