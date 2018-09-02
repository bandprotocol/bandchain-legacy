#include "context_map.h"


uint64_t ContextMap::height() const
{
  // TODO
  return 0;
}

Hash ContextMap::root() const
{
  // TODO
  return {};
}

void ContextMap::commit()
{
  // TODO
}

std::pair<std::string, bool> ContextMap::try_get(const Hash& key) const
{
  if (auto it = data.find(key); it != data.end()) {
    return {it->second, true};
  }
  return {{}, false};
}

std::string ContextMap::get(const Hash& key) const
{
  if (auto it = data.find(key); it != data.end()) {
    return it->second;
  }
  throw Error("Context::get: key {} does not exist", key);
}

bool ContextMap::check(const Hash& key) const
{
  return data.find(key) != data.end();
}

void ContextMap::add(const Hash& key, const std::string value)
{
  if (!data.try_emplace(key, value).second) {
    throw Error("Context::add: key {} already exists", key);
  }
}

void ContextMap::set(const Hash& key, const std::string value)
{
  data[key] = value;
}
