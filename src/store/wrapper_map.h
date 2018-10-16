#pragma once

#include <nonstd/optional.hpp>
#include <unordered_map>

#include "inc/essential.h"
#include "store/contract.h"
#include "store/global.h"

template <typename Key, typename Value>
class WrapperMap
{
public:
  WrapperMap(Contract& contract)
      : nonce(contract.increment_then_get())
      , flush(contract.flush)
      , m_addr(contract.m_addr)
  {
  }

  ~WrapperMap()
  {
    if (flush) {
      DEBUG(log, "Changed item list");
      for (auto& [key, value] : cache) {
        if (value.written) {
          DEBUG(log, "Address {} has {}", key, value.val);
          Global::get().m_ctx->store.put(sha256(m_addr, nonce, key),
                                         Buffer::serialize<Value>(value.val));
        }
      }
    }
  }

  Value at(const Key& key) const
  {
    if (auto it = cache.find(key); it != cache.end())
      return it->second.val;
    else {
      auto result = Global::get().m_ctx->store.get(sha256(m_addr, nonce, key));
      if (result) {
        return cache
            .emplace(key, ValueAndWritten{Buffer::deserialize<Value>(*result),
                                          false})
            .first->second.val;
      } else
        throw Error("Failed to get value");
    }
  }

  nonstd::optional<Value> try_at(const Key& key) const
  {
    if (auto it = cache.find(key); it != cache.end())
      return it->second.val;
    else {
      auto result = Global::get().m_ctx->store.get(sha256(m_addr, nonce, key));
      if (result) {
        return cache
            .emplace(key, ValueAndWritten{Buffer::deserialize<Value>(*result),
                                          false})
            .first->second.val;
      } else
        return nonstd::nullopt;
    }
  }

  void set(const Key& key, const Value& val) { cache[key] = {val, true}; }

  Value& operator[](const Key& key)
  {
    if (auto it = cache.find(key); it != cache.end()) {
      it->second.written = true;
      return it->second.val;
    } else {
      auto result = Global::get().m_ctx->store.get(sha256(m_addr, nonce, key));
      if (result) {
        return cache
            .emplace(key,
                     ValueAndWritten{Buffer::deserialize<Value>(*result), true})
            .first->second.val;
      } else {
        return cache.emplace(key, ValueAndWritten{Value{}, true})
            .first->second.val;
      }
    }
  }

  bool contains(const Key& key) const
  {
    if (cache.count(key) == 1)
      return true;
    else {
      auto result = Global::get().m_ctx->store.get(sha256(m_addr, nonce, key));
      if (result) {
        Value data = Buffer::deserialize<Value>(*result);
        cache.emplace(key, ValueAndWritten{data, false});
        return true;
      } else
        return false;
    }
  }

private:
  struct ValueAndWritten {
    Value val;
    bool written;
  };
  const uint256_t nonce;
  const bool& flush;
  mutable std::unordered_map<Key, ValueAndWritten> cache;
  const Address m_addr;
  static inline auto log = logger::get("wmap");
};
