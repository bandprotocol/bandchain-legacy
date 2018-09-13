#include "context_map.h"

ContextMap::ContextMap() { DEBUG(log, "CONTEXT_MAP_INIT"); }

Object* ContextMap::get(const ContextKey& key) const
{
  if (auto it = data.find(key); it != data.end()) {
    return it->second.get();
  }
  return nullptr;
}

bool ContextMap::check(const ContextKey& key) const
{
  return data.find(key) != data.end();
}

void ContextMap::add(const ContextKey& key, std::unique_ptr<Object> obj)
{
  auto [it, ok] = data.try_emplace(key, std::move(obj));
  if (!ok) {
    throw Error("Context::add: key {} already exists", key);
  }
}
