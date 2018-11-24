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

/// Shorthand marcro to define data mapping field inside of contract.
#define DATAMAP(VAL, NAME) DataMap<VAL> NAME{storage, key + "/" + #NAME + "/"};

/// DataMap is a wrapper over a key-value like lookup interface. It does not
/// maintain any data on its own, but rather facilitate key-generation for the
/// underlying data. Note that this means DataMap does not support iteration.
template <typename Value>
class DataMap
{
public:
  /// Create this data wrapper based on the given key and the storage reference.
  DataMap(Storage& _storage, const std::string& _key)
      : storage(_storage)
      , baseKey(_key)
  {
  }

  /// Copy and move don't make much sense here and can lead to weird bugs.
  /// Better to just disable them both.
  DataMap(const DataMap& dataMap) = delete;
  DataMap(DataMap&& dataMap) = delete;

  /// Operation[] returns the reference to the underlying mapping. The caller
  /// can use it however they won't. DataMap ensures that the created mapping
  /// won't get destroyed until this DataMap is destroyed. Both const and
  /// non-const versions are provided.
  template <typename T>
  const Value& operator[](const T& key) const
  {
    const std::string keyString = key.to_string();

    if (auto it = cache.find(keyString); it != cache.end())
      return it->second;

    return cache
        .emplace(std::piecewise_construct, std::forward_as_tuple(keyString),
                 std::forward_as_tuple(storage, baseKey + keyString))
        .first->second;
  }

  template <typename T>
  Value& operator[](const T& key)
  {
    return const_cast<Value&>(
        static_cast<const DataMap*>(this)->operator[](key));
  }

private:
  /// Reference to the storage layer.
  Storage& storage;

  /// The key to which this wrapper use to access data.
  const std::string baseKey;

  /// The map to keep track of 'active' data values. Storing it in the map means
  /// their destructors won't get called until this DataMap is destructed.
  mutable std::unordered_map<std::string, Value> cache;
};
