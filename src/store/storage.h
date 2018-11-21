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
#include "store/contract.h"

/// Storage is an interface for connecting to key-value undelying store. As of
/// current, there are two implementations: one is backed by memory (C++
/// std::unordered_map) and one is backed by RocksDB.
class Storage
{
public:
  /// Clear all the pending cache, discarding all the changes.
  void reset();

  /// Flush all the cached information into the storage. Call commit under the
  /// hood, while ensuring that shouldFlush returns true during the process.
  void flush();

  /// Return a boolean indicating whether the storage is currently flushing.
  bool shouldFlush() const;

  /// Load the contract of type T and location key. Throw if key does not exists
  /// or if the contract there is not of type T.
  template <typename T, typename KEY>
  T& load(const KEY& key)
  {
    const std::string prefixedKey = T::KeyPrefix + key.to_string();

    if (auto ptr = getContract<T>(prefixedKey); ptr != nullptr)
      return *ptr;

    throw Error("Storage::load: contract key {} does not exist", prefixedKey);
  }

  /// Create a new contract of type T at location key. Also initialize the
  /// contract by calling its init function with the provided arguments.
  template <typename T, typename KEY, typename... Args>
  T& create(const KEY& key, Args&&... args)
  {
    const std::string prefixedKey = T::KeyPrefix + key.to_string();

    if (auto ptr = getContract<T>(prefixedKey); ptr != nullptr)
      throw Error("Storage::create: contract key {} already exists",
                  prefixedKey);

    auto uniq = std::make_unique<T>(*this, prefixedKey);
    auto raw = uniq.get();

    cache[prefixedKey] = std::move(uniq);
    put(prefixedKey, prefixedKey);

    raw->init(std::forward<Args>(args)...);
    return *raw;
  }

public:
  /// Return the value mapped to the key if exists, otherwise return nullopt.
  virtual nonstd::optional<std::string> get(const std::string& key) const = 0;

  /// Map the given key to the given value, using upsert semantics.
  virtual void put(const std::string& key, const std::string& val) = 0;

  /// Delete the given key from the storage. May throw if key does not exist.
  virtual void del(const std::string& key) = 0;

  /// Issue commit command to the storage.
  virtual void commit() = 0;

  /// Switch to check mode. Changes in this mode are discarded at commit.
  virtual void switchToCheck() = 0;

  /// Switch to apply mode. Changes in this mode are REAL.
  virtual void switchToApply() = 0;

private:
  template <typename T>
  T* getContract(const std::string& prefixedKey)
  {
    if (auto it = cache.find(prefixedKey); it != cache.end())
      return it->second->as<T>();

    auto storeValue = get(prefixedKey);
    if (!storeValue.has_value())
      return nullptr;

    if (*storeValue != prefixedKey)
      throw Error("Storage::getContract: contract key {} is not consistent "
                  "with the value {}",
                  prefixedKey, *storeValue);

    auto uniq = std::make_unique<T>(*this, prefixedKey);
    auto raw = uniq.get();

    cache[prefixedKey] = std::move(uniq);
    return raw;
  }

private:
  /// A boolean to indicate whether this storage is in the process of flushing
  /// the cached data in to the peristent store.
  bool isFlushing = false;

  /// A map keeping all the pending contracts. The changes made in those
  /// contracts are put to the storage after the contracts are destructed.
  std::unordered_map<std::string, std::unique_ptr<Contract>> cache;
};
