#pragma once

#include <unordered_map>

#include "inc/essential.h"
#include "util/bytes.h"

class Object
{
public:
  Object(const Object&) = delete;

  Object(const ContextKey& _key)
      : key(_key)
  {
  }

  virtual ~Object() {}

  virtual void debug_save() const = 0;

  template <typename T>
  T* as()
  {
    T* result = dynamic_cast<T*>(this);
    if (result == nullptr)
      throw Error("Invalid object cast");
    return result;
  }

public:
  const ContextKey key;
};

class Context
{
public:
  virtual ~Context(){};

  void reset();
  void flush();

  template <typename T>
  T& get(const ContextKey& key)
  {
    if (auto ptr = get_context_impl<T>(key); ptr != nullptr)
      return *ptr;
    throw Error("ContextKey {} does not exist", key);
  }

  template <typename T, typename... Args>
  T& get_or_create(const ContextKey& key, Args&&... args)
  {
    if (auto ptr = get_context_impl<T>(key); ptr != nullptr)
      return *ptr;
    return create_context_impl<T>(key, std::forward<Args>(args)...);
  }

  template <typename T, typename... Args>
  T& create(const ContextKey& key, Args&&... args)
  {
    if (auto ptr = get_context_impl<T>(key); ptr != nullptr)
      throw Error("ContextKey {} already exists", key);
    return create_context_impl<T>(key, std::forward<Args>(args)...);
  }

private:
  template <typename T>
  T* get_context_impl(const ContextKey& key)
  {
    if (auto it = cache.find(key); it != cache.end())
      return it->second.get()->as<T>();

    auto impl_ptr = get_impl(key);
    if (impl_ptr == nullptr)
      return nullptr;

    auto uniq = std::make_unique<T>(*impl_ptr->as<T>());
    auto raw = uniq.get();
    cache[key] = std::move(uniq);

    return raw;
  }

  template <typename T, typename... Args>
  T& create_context_impl(const ContextKey& key, Args&&... args)
  {
    auto uniq = std::make_unique<T>(key, std::forward<Args>(args)...);
    auto raw = uniq.get();
    auto [it, ok] = cache.try_emplace(key, std::move(uniq));
    if (!ok)
      throw Failure("ContextKey {} already exists in cache", key);
    return *raw;
  }

protected:
  /// To be overriden by Context implementation
  virtual Object* get_impl(const ContextKey& key) const = 0;
  virtual void add_impl(std::unique_ptr<Object> obj) = 0;

  /// Log instance
  static inline auto log = logger::get("context");

private:
  std::unordered_map<ContextKey, std::unique_ptr<Object>> cache;
};
