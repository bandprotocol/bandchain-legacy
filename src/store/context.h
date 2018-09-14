#pragma once

#include "inc/essential.h"
#include "util/bytes.h"

class Object
{
public:
  Object(const ContextKey& _key)
      : key(_key)
  {
  }

  virtual ~Object() {}

  template <typename T>
  T& as()
  {
    T* result = dynamic_cast<T*>(this);
    if (result == nullptr)
      throw Error("Invalid object cast");
    return *result;
  }

  const ContextKey key;
};

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

  virtual Object* get(const ContextKey& key) const = 0;

  virtual bool check(const ContextKey& key) const = 0;

  virtual void add(std::unique_ptr<Object> obj) = 0;

  template <typename T>
  T& get_as(const ContextKey& key)
  {
    return get(key)->as<T>();
  }

  template <typename T, typename... Args>
  T& create(const ContextKey& key, Args&&... args)
  {
    auto uniq = std::make_unique<T>(key, std::forward<Args>(args)...);
    auto raw = uniq.get();
    add(std::move(uniq));
    return *raw;
  }

  template <typename T, typename... Args>
  T& get_or_create(const ContextKey& key, Args&&... args)
  {
    if (auto ptr = get(key); ptr != nullptr)
      return ptr->as<T>();

    return create<T>(key, std::forward<Args>(args)...);
  }

protected:
  static inline auto log = logger::get("context");
};
