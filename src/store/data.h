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

#include <enum/enum.h>
#include <nonstd/optional.hpp>

#include "inc/essential.h"
#include "store/storage.h"
#include "util/buffer.h"

/// Shorthand marcro to define data field inside of contract.
#define DATA(TYPE, NAME) Data<TYPE> NAME{storage, key + "/" + #NAME};

/// The status of data inside the wrapper. Data will be flushed to database
/// following this status.
ENUM(DataCacheStatus, uint8_t, Unchanged, Changed, Erased)

/// Data is a wrapper for any data type on top of a key-value storage layer.
/// It is responsible for loading data from the underlying store, saving changes
/// and flushing the change to the store on destruction.
template <typename T>
class Data
{
public:
  /// Create this data wrapper based on the given key and the storage reference.
  Data(Storage& _storage, const std::string& _key)
      : storage(_storage)
      , key(_key)
  {
  }

  /// On destruction, flush all changes made to this wrapper into the persistent
  /// key-value storage.
  ~Data()
  {
    if (!storage.shouldFlush())
      return;

    switch (status) {
      case +DataCacheStatus::Unchanged:
        break;
      case +DataCacheStatus::Changed: {
        DEBUG(log, "PUT {} -> {}", key, *cache);
        storage.put(key, Buffer::serialize<T>(*cache));
        break;
      }
      case +DataCacheStatus::Erased: {
        DEBUG(log, "DEL {}", key);
        storage.del(key);
        break;
      }
    }
  }

  /// Copy and move don't make much sense here and can lead to weird bugs.
  /// Better to just disable them both.
  Data(const Data& data) = delete;
  Data(Data&& data) = delete;

  /// Operator+ returns the underlying data inside this wrapper. The wrapper
  /// will need to fetch data from the storage if it's not available.
  T operator+() const
  {
    if (status == DataCacheStatus::Erased)
      throw Error("Data::operator+: cannot access deleted object");

    if (cache)
      return *cache;

    nonstd::optional<std::string> result = storage.get(key);

    if (result) {
      cache = Buffer::deserialize<T>(*result);
      return *cache;
    }

    if constexpr (!std::is_enum_v<T>) {
      cache = T{};
      return *cache;
    }

    throw Error("Data::operator+: construct object without default ctor");
  }

  /// Check if the data actually has the underlying value backed in storage.
  bool exist()
  {
    if (status == DataCacheStatus::Erased)
      return false;

    if (cache)
      return true;

    return storage.get(key).has_value();
  }

  /// Update the value. Note that it does not actually make the change into the
  /// store until the flush occurs.
  void operator=(const T& value)
  {
    cache = value;
    status = DataCacheStatus::Changed;
  }

  /// Mark the value as erased. Similar to operator=, this is not reflect in the
  /// storage until data destruction.
  void erase()
  {
    cache = nonstd::nullopt;
    status = DataCacheStatus::Erased;
  }

private:
  /// Reference to the storage layer.
  Storage& storage;

  /// The key to which this wrapper use to access data.
  const std::string key;

  /// The status of the cache below.
  DataCacheStatus status = DataCacheStatus::Unchanged;

  /// The cache value. This will be flushed once this wrapper is destroyed.
  mutable nonstd::optional<T> cache;

  /// Static logger for this class.
  static inline auto log = logger::get("data");
};
