#pragma once

#include "util/bytes.h"

class Context
{
public:
  Context();

  /// Get the current blockchain height.
  uint64_t height() const;

  /// Get the current merkle root of the blockchain.
  Hash root() const;

  /// Commit all changes to persistent data store, and increment the block 
  /// height by one.
  void commit();

  /// Return the data stored at the given key, and boolean indicating the 
  /// success.
  std::pair<std::string, bool> try_get(const Hash& key) const;

  /// Return the data stored at the given key. Throw of the key does not exist.
  std::string get(const Hash& key) const;

  /// Return true iff the key exists in the database.
  bool check(const Hash& key) const;

  /// Add the data stored at the given key. Throw if the key already exists.
  void add(const Hash& key, const std::string value);

  /// Similar to add, but don't throw.
  void set(const Hash& key, const std::string value);

private:
  std::unordered_map<Hash, std::string> data;
};
