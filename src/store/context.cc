#include "context.h"

#include <rocksdb/db.h>

Context::Context() {}

uint64_t Context::height() const
{
  // TODO
  return 0;
}

Hash Context::root() const
{
  // TODO
  return {};
}

void Context::commit()
{
  // TODO
}

std::pair<std::string, bool> Context::try_get(const Hash& key) const
{
  if (auto it = data.find(key); it != data.end()) {
    return {it->second, true};
  }
  return {{}, false};
}

std::string Context::get(const Hash& key) const
{
  if (auto it = data.find(key); it != data.end()) {
    return it->second;
  }
  throw Error("Context::get: key {} does not exist", key);
}

bool Context::check(const Hash& key) const
{
  return data.find(key) != data.end();
}

void Context::add(const Hash& key, const std::string value)
{
  if (!data.try_emplace(key, value).second) {
    throw Error("Context::add: key {} already exists", key);
  }
}

void Context::set(const Hash& key, const std::string value)
{
  data[key] = value;
}
