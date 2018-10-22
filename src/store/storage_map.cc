#include "storage_map.h"

#include "crypto/sha256.h"

void StorageMap::put(const Hash& key, const std::string& val)
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

void StorageMap::save_protected_key(const std::string& key,
                                    const std::string& val)
{
  data[sha256(gsl::make_span(key))] = val;
}

nonstd::optional<std::string>
StorageMap::get_protected_key(const std::string& key)
{
  return get(sha256(gsl::make_span(key)));
}
