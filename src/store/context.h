#pragma once

#include <unordered_map>

#include "inc/essential.h"
#include "util/bytes.h"
#include "util/iban.h"

class Object
{
public:
  virtual ~Object() {}
  virtual const Address& get_key() const = 0;

  virtual void debug_create() const = 0;
  virtual void debug_save() const = 0;

  template <typename T>
  T* as()
  {
    T* result = dynamic_cast<T*>(this);
    if (result == nullptr)
      throw Error("Invalid object cast");
    return result;
  }
};

template <IBANType::_enumerated IBAN_TYPE>
class ObjectImpl : public Object
{
public:
  ObjectImpl(const ObjectImpl&) = default;
  ObjectImpl(const IBANAddrBase<IBAN_TYPE>& _key)
      : key(_key)
  {
  }

  const Address& get_key() const final { return key; }

protected:
  IBANAddrBase<IBAN_TYPE> key;
};

class Context
{
public:
  virtual ~Context(){};

  void reset();
  void flush();

  template <typename T>
  T& get(const Address& key)
  {
    if (auto ptr = get_context_impl<T>(key); ptr != nullptr)
      return *ptr;
    throw Error("Address {} does not exist", key.to_hex_string());
  }

  template <typename T, typename Key, typename... Args>
  T& get_or_create(const Key& key, Args&&... args)
  {
    if (auto ptr = get_context_impl<T>(key); ptr != nullptr)
      return *ptr;
    return create_context_impl<T>(key, std::forward<Args>(args)...);
  }

  template <typename T, typename Key, typename... Args>
  T& create(const Key& key, Args&&... args)
  {
    if (auto ptr = get_context_impl<T>(key); ptr != nullptr)
      throw Error("Address {} already exists", key.to_hex_string());
    return create_context_impl<T>(key, std::forward<Args>(args)...);
  }

private:
  template <typename T>
  T* get_context_impl(const Address& key)
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

  template <typename T, typename Key, typename... Args>
  T& create_context_impl(const Key& key, Args&&... args)
  {
    auto uniq = std::make_unique<T>(key, std::forward<Args>(args)...);
    uniq->debug_create();

    auto raw = uniq.get();
    bool ok = cache.try_emplace(key, std::move(uniq)).second;
    if (!ok)
      throw Failure("Address {} already exists in cache", key.to_hex_string());
    return *raw;
  }

protected:
  /// To be overriden by Context implementation
  virtual Object* get_impl(const Address& key) const = 0;
  virtual void add_impl(std::unique_ptr<Object> obj) = 0;

  /// Log instance
  static inline auto log = logger::get("context");

private:
  std::unordered_map<Address, std::unique_ptr<Object>> cache;
};
