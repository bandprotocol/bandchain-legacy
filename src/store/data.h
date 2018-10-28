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

#include "inc/essential.h"
#include "store/contract.h"
#include "store/global.h"

ENUM(CacheStatus, uint16_t, Unchanged = 0, Changed = 1, Erased = 2)

template <typename T>
class Data
{
public:
  Data(const Hash& _key)
      : key(_key)
      , status(CacheStatus::Unchanged)
  {
  }

  Data(const Data& data) = delete;
  Data(Data&& data) = delete;

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
        cache = Buffer::deserialize<T>(*result);
      } else {
        if constexpr (!std::is_enum_v<T>) {
          cache = T{};
        } else {
          throw Error("Cannot create default constructor");
        }
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
        cache = Buffer::deserialize<T>(*result);
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
