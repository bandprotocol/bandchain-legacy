// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the LICENSE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#pragma once

#include <nonstd/optional.hpp>
#include <unordered_map>

#include "inc/essential.h"

template <typename Key, typename Value>
class Mapping
{
public:
  Mapping(const Hash& _hash)
      : hash(_hash)
  {
  }

  Mapping(const Mapping& w) = delete;
  Mapping(Mapping&& w) = delete;

  // ~Mapping() { NOCOMMIT_LOG("Clear cache map"); }

  // Value at(const Key& key) const
  // {
  //   if (auto it = cache.find(key); it != cache.end())
  //     return it->second.val;
  //   else {
  //     auto result = Global::get().m_ctx->store.get(sha256(m_addr, nonce,
  //     key)); if (result) {
  //       return cache
  //           .emplace(key,
  //           ValueAndWritten{Buffer::deserialize<Value>(*result),
  //                                         false})
  //           .first->second.val;
  //     } else
  //       throw Error("Failed to get value");
  //   }
  // }

  // nonstd::optional<Value> try_at(const Key& key) const
  // {
  //   if (auto it = cache.find(key); it != cache.end())
  //     return it->second;
  //   else {
  //     auto result = Global::get().m_ctx->store.get(sha256(hash, key));
  //     if (result) {
  //       return cache.emplace(key, Value{sha256(hash, key)}).first->second;
  //     } else
  //       return nonstd::nullopt;
  //   }
  // }

  // void set(const Key& key, const Value& val) { cache[key] = {val, true}; }

  Value& operator[](const Key& key) const
  {
    if (auto it = cache.find(key); it != cache.end()) {
      return it->second;
    } else {
      return cache.emplace(key, sha256(hash, key)).first->second;
    }
  }

private:
  // struct ValueAndWritten {
  //   Value val;
  //   bool written;
  // };
  const Hash hash;
  mutable std::unordered_map<Key, Value> cache;
  static inline auto log = logger::get("wmap");
};
