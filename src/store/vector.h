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

#include "inc/essential.h"
#include "store/storage.h"
#include "util/buffer.h"
#include "util/bytes.h"

/// Shorthand macro to define Vector field inside of contract.
#define VECTOR(VAL, NAME) Vector<VAL> NAME{storage, key + "/" + #NAME + "/"};

template <typename T>
class Vector
{
public:
  Vector(Storage& _storage, const std::string& _key)
      : storage(_storage)
      , baseKey(_key)
  {
    auto result = storage.get(baseKey);
    if (result) {
      mSize = Buffer::deserialize<uint256_t>(*result);
    } else {
      mSize = 0;
    }
  }

  ~Vector()
  {
    if (storage.shouldFlush()) {
      if (isDestroyed) {
        DEBUG(log, "DELETE VECTOR {}", baseKey);
        storage.del(baseKey);
        for (uint256_t i = 0; i < mSize; i++) {
          storage.del(baseKey + i.str());
        }
      } else {
        storage.put(baseKey, Buffer::serialize<uint256_t>(mSize));

        // Save every member in cache to storage.
        for (auto& [idx, value] : cache) {
          storage.put(baseKey + idx.str(), Buffer::serialize<T>(value));
          DEBUG(log, "PUT {}", baseKey + idx.str());
        }
      }
    }
  }

  T operator[](const uint256_t& idx) const
  {
    if (isDestroyed) {
      throw Error("Vector has been destroyed.");
    }
    if (idx >= mSize) {
      throw Error("index out of range");
    }
    if (auto it = cache.find(idx); it != cache.end()) {
      return it->second;
    } else {
      auto result = storage.get(baseKey + idx.str());
      if (!result) {
        throw Failure("Value missing at index {}.", idx);
      }
      T value = Buffer::deserialize<T>(*result);
      return cache.emplace(idx, value).first->second;
    }
  }

  void pushBack(const T& value)
  {
    if (isDestroyed) {
      throw Error("Vector has been destroyed.");
    }
    cache.emplace(mSize, value);
    mSize++;
  }

  void destroy()
  {
    isDestroyed = true;
  }

  uint256_t size() const
  {
    return mSize;
  }

  T back() const
  {
    if (mSize == 0)
      throw Error("Cannot get last element.");
    return operator[](mSize - 1);
  }

  uint256_t lowerBoundIndex(const T& value) const
  {
    uint256_t st = 0;
    uint256_t ed = mSize;
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
  /// Reference to the storage layer.
  Storage& storage;

  /// The key to which this Vector use to access data.
  const std::string baseKey;

  /// Size of vector
  uint256_t mSize;

  /// The map to keep track of 'active' member in Vector. All member in cache
  /// are saved when this Vector is deconstructed.
  mutable std::unordered_map<uint256_t, T> cache;

  /// Boolean tells that Vector have been destroyed yet.
  bool isDestroyed = false;

  /// Static logger for this class.
  static inline auto log = logger::get("vector");
};
