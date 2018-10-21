#pragma once
#include <nonstd/optional.hpp>

#include "inc/essential.h"
#include "store/contract.h"
#include "store/global.h"

BETTER_ENUM(CacheStatus, uint16_t, Unchanged = 0, Changed = 1, Erased = 2)

template <typename T>
class Data
{
public:
  Data(const Hash& _key)
      : key(_key)
      , status(CacheStatus::Unchanged)
  {
  }

  Data(const Data& w) = delete;
  Data(Data&& w) = delete;

  ~Data()
  {
    if (Global::get().flush) {
      if (status == CacheStatus::Erased) {
        Global::get().m_ctx->store.del(key);
      } else if (status == CacheStatus::Changed)
        Global::get().m_ctx->store.put(key, Buffer::serialize<T>(*cache));
    }
  }

  T operator+() const
  {
    if (status == CacheStatus::Erased)
      throw Error("Object has erased.");

    if (cache) {
      return *cache;
    } else {
      auto result = Global::get().m_ctx->store.get(key);
      if (result) {
        // cache = Buffer::deserialize<T>(*result);
        Buffer buf{gsl::make_span(*result)};
        buf >> *cache;
      } else {
        if constexpr (std::is_default_constructible_v<T>) {
          cache = T{};
        } else
          throw Error("Cannot create default constructor");
      }
      return *cache;
    }
  }

  // void operator=(T value)
  // {
  //   cache = value;
  //   status = CacheStatus::Changed;
  // }

  void operator=(const T& value)
  {
    cache = value;
    status = CacheStatus::Changed;
  }

  void erase()
  {
    cache = nonstd::nullopt;
    status = CacheStatus::Erased;
  }

  bool exist()
  {
    if (status == CacheStatus::Erased)
      return false;

    if (cache) {
      return true;
    } else {
      auto result = Global::get().m_ctx->store.get(key);
      if (result) {
        // cache = Buffer::deserialize<T>(*result);
        Buffer buf{gsl::make_span(*result)};
        buf >> *cache;
        return true;
      } else {
        return false;
      }
    }
  }

private:
  const Hash key;
  CacheStatus status = CacheStatus::Unchanged;
  mutable nonstd::optional<T> cache{};
};
