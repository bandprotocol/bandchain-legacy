#pragma once

#include <nonstd/optional.hpp>

#include "inc/essential.h"
#include "util/bytes.h"

class Storage
{
public:
  virtual ~Storage() {}

  // Save or create value to key.
  virtual void put(const Hash& key, const std::string val) = 0;

  // Get value from key or return nullopt if not exist.
  virtual nonstd::optional<std::string> get(const Hash& key) const = 0;

  // Delete key from storage.
  virtual void del(const Hash& key) = 0;
};
