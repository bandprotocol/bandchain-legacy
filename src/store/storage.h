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

/// TODO
class Storage
{
public:
  /// TODO
  void reset();

  /// TODO
  void flush();

  /// TODO
  bool shouldFlush() const;

  ///
  virtual void commit() = 0;

  /// TODO
  template <typename T>
  T& load(const std::string& key)
  {
    const std::string prefixedKey = T::KeyPrefix + key;

    if (auto ptr = getContract<T>(prefixedKey); ptr != nullptr)
      return *ptr;

    throw Error("Storage::load: contract key {} does not exist", prefixedKey);
  }

  /// TODO
  template <typename T, typename... Args>
  T& create(const std::string& key, Args&&... args)
  {
    const std::string prefixedKey = T::KeyPrefix + key;

    if (auto ptr = getContract<T>(prefixedKey); ptr == nullptr)
      throw Error("Storage::create: contract key {} already exists",
                  prefixedKey);

    auto uniq = std::make_unique<T>(*this, prefixedKey);
    auto raw = uniq.get();

    uniq->init(std::forward<Args>(args)...);
    put(prefixedKey, prefixedKey);

    cache[prefixedKey] = std::move(uniq);
    return *raw;
  }

public:
  ///
  virtual nonstd::optional<std::string> get(const std::string& key) const = 0;

  ///
  virtual void put(const std::string& key, const std::string& val) = 0;

  ///
  virtual void del(const std::string& key) = 0;

  ///
  virtual void switchToCheck() = 0;

  ///
  virtual void switchToApply() = 0;

private:
  ///
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

  ///
  std::unordered_map<std::string, std::unique_ptr<Contract>> cache;
};
