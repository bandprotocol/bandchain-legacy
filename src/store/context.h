#pragma once

#include <unordered_map>

#include "inc/essential.h"
#include "store/contract.h"
#include "util/buffer.h"
#include "util/bytes.h"

class Context
{
public:
  friend class Contract;

  virtual ~Context(){};

  void call(Buffer& in_buf, Buffer* out_buf = nullptr);

  void reset();
  void flush();

  template <typename T>
  T& get(const Address& key)
  {
    if (auto ptr = get_context_impl(key); ptr != nullptr)
      return *(ptr->as<T>());
    throw Error("Address {} does not exist", key);
  }

  template <typename T, typename... Args>
  T& create(Args&&... args)
  {
    return create_context_impl<T>(std::forward<Args>(args)...);
  }

private:
  Contract* get_context_impl(const Address& key)
  {
    if (auto it = cache.find(key); it != cache.end())
      return it->second.get();

    auto impl_ptr = get_impl(key);
    if (impl_ptr == nullptr)
      return nullptr;

    auto uniq = impl_ptr->clone();
    auto raw = uniq.get();
    cache[key] = std::move(uniq);

    return raw;
  }

  template <typename T, typename... Args>
  T& create_context_impl(Args&&... args)
  {
    auto uniq = std::make_unique<T>(std::forward<Args>(args)...);
    uniq->debug_create();

    auto raw = uniq.get();
    bool ok = cache.try_emplace(raw->m_addr, std::move(uniq)).second;
    if (!ok)
      throw Failure("Address {} already exists in cache", raw->m_addr);

    return *raw;
  }

protected:
  /// To be overriden by Context implementation
  virtual Contract* get_impl(const Address& key) const = 0;
  virtual void add_impl(std::unique_ptr<Contract> obj) = 0;

  /// Log instance
  static inline auto log = logger::get("context");

private:
  std::unordered_map<Address, std::unique_ptr<Contract>> cache;
};
