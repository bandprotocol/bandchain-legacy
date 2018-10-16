#include "storage_map.h"

void StorageMap::put(const Hash& key, const std::string val)
{
  data[key] = val;
}

nonstd::optional<std::string> StorageMap::get(const Hash& key) const
{
  if (auto it = data.find(key); it != data.end())
    return it->second;
  else
    return nonstd::nullopt;
}

void StorageMap::del(const Hash& key) { data.erase(key); }
