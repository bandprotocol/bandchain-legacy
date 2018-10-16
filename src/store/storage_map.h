#pragma once

#include <nonstd/optional.hpp>
#include <unordered_map>

#include "store/storage.h"

class StorageMap : public Storage
{
public:
  void put(const Hash& key, const std::string val) final;
  nonstd::optional<std::string> get(const Hash& key) const final;
  void del(const Hash& key) final;

public:
  std::unordered_map<Hash, std::string> data;
};
