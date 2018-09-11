#pragma once

#include "inc/essential.h"
#include "util/bytes.h"

class Context
{
public:
  virtual ~Context(){};

  /// Get the current blockchain height.
  virtual uint64_t height() const = 0;

  /// Get the current merkle root of the blockchain.
  virtual Hash root() const = 0;

  /// Commit all changes to persistent data store, and increment the block
  /// height by one.
  virtual void commit() = 0;

  /// Return the data stored at the given key, and boolean indicating the
  /// success.
  virtual std::pair<std::string, bool> try_get(const Hash& key) const = 0;

  /// Return the data stored at the given key. Throw of the key does not exist.
  virtual std::string get(const Hash& key) const = 0;

  /// Return true iff the key exists in the database.
  virtual bool check(const Hash& key) const = 0;

  /// Add the data stored at the given key. Throw if the key already exists.
  virtual void add(const Hash& key, const std::string value) = 0;

  /// Similar to add, but don't throw.
  virtual void set(const Hash& key, const std::string value) = 0;

protected:
  static inline auto log = logger::get("context");
};
