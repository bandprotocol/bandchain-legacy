#include "context_map.h"

ContextMap::ContextMap() { DEBUG(log, "CONTEXT_MAP_INIT"); }

Object* ContextMap::get(const Hash& key) const
{
  if (auto it = data.find(key); it != data.end()) {
    return it->second.get();
  }
  return nullptr;
}

bool ContextMap::check(const Hash& key) const
{
  return data.find(key) != data.end();
}

void ContextMap::add(const Hash& key, std::unique_ptr<Object> obj)
{
  if (!data.try_emplace(key, std::move(obj)).second) {
    throw Error("Context::add: key {} already exists", key);
  }
}
