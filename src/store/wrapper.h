#pragma once
#include <nonstd/optional.hpp>

#include "inc/essential.h"
#include "store/contract.h"
#include "store/global.h"

template <typename T>
class Wrapper
{
public:
  Wrapper(Contract& contract)
      : key(sha256(contract.m_addr, contract.increment_then_get()))
      , flush(contract.flush)
      , rewritten(false)
  {
  }

  ~Wrapper()
  {
    if (flush && cache && rewritten) {
      Global::get().m_ctx->store.put(key, Buffer::serialize<T>(*cache));
    }
  }

  T get() const
  {
    if (cache)
      return *cache;
    else {
      auto result = Global::get().m_ctx->store.get(key);
      if (result) {
        cache = Buffer::deserialize<T>(*result);
        return *cache;
      } else
        throw Error("Failed to get value");
    }
  }

  void set(T value)
  {
    cache = value;
    rewritten = true;
  }

private:
  const Hash key;
  const bool& flush;
  bool rewritten;
  mutable nonstd::optional<T> cache{};
};
