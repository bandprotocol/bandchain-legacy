#pragma once

#include "inc/essential.h"
#include "store/object.h"
#include "util/bytes.h"

class Context
{
public:
  virtual ~Context(){};

  // /// Get the current blockchain height.
  // virtual uint64_t height() const = 0;

  // /// Get the current merkle root of the blockchain.
  // virtual Hash root() const = 0;

  // /// Commit all changes to persistent data store, and increment the block
  // /// height by one.
  // virtual void commit() = 0;

  virtual Object* get(const Hash& key) const = 0;

  virtual bool check(const Hash& key) const = 0;

  virtual void add(const Hash& key, std::unique_ptr<Object> obj) = 0;

protected:
  static inline auto log = logger::get("context");
};
