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

#include <unordered_map>

#include "crypto/sha256.h"
#include "inc/essential.h"
#include "store/global.h"
#include "util/buffer.h"
#include "util/bytes.h"

template <typename T>
class Vector
{
public:
  Vector(const Hash& _parent_hash)
      : parent_hash(_parent_hash)
  {
    auto result = Global::get().m_ctx->store.get(parent_hash);
    if (result) {
      Buffer buf{gsl::make_span(*result)};
      m_size = buf.read_all<uint256_t>();
    } else {
      m_size = 0;
    }
  }

  ~Vector()
  {
    if (Global::get().flush) {
      if (is_destroyed) {
        Global::get().m_ctx->store.del(parent_hash);
        for (uint256_t i = 0; i < m_size; i++) {
          Global::get().m_ctx->store.del(sha256(parent_hash, i));
        }
      } else {
        Buffer buf;
        Global::get().m_ctx->store.put(parent_hash,
                                       Buffer::serialize<uint256_t>(m_size));

        // Save every member in cache to storage.
        for (auto& [idx, value] : cache) {
          Global::get().m_ctx->store.put(sha256(parent_hash, idx),
                                         Buffer::serialize<T>(value));
        }
      }
    }
  }

  T operator[](const uint256_t& idx) const
  {
    if (is_destroyed) {
      throw Error("Vector has been destroyed.");
    }
    if (idx >= m_size) {
      throw Error("index out of range");
    }
    if (auto it = cache.find(idx); it != cache.end()) {
      return it->second;
    } else {
      auto result = Global::get().m_ctx->store.get(sha256(parent_hash, idx));
      if (!result) {
        throw Failure("Value missing at index {}.", idx);
      }

      T value = Buffer::deserialize<T>(*result);
      return cache.emplace(idx, value).first->second;
    }
  }

  void push_back(const T& value)
  {
    if (is_destroyed) {
      throw Error("Vector has been destroyed.");
    }
    cache.emplace(m_size, value);
    m_size++;
  }

  void destroy() { is_destroyed = true; }

  uint256_t size() const { return m_size; }

  T back() const { return operator[](m_size - 1); }

  uint256_t lower_bound_index(const T& value) const
  {
    uint256_t st = 0;
    uint256_t ed = m_size;
    while (st < ed) {
      uint256_t mid = (st + ed) / 2;
      if (operator[](mid) >= value) {
        ed = mid;
      } else {
        st = mid + 1;
      }
    }
    return st;
  }

private:
  const Hash parent_hash;
  uint256_t m_size;
  mutable std::unordered_map<uint256_t, T> cache;
  bool is_destroyed = false;
};
