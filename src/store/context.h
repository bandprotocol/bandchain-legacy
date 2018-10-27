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
#include "store/contract.h"
#include "store/storage.h"
#include "util/buffer.h"
#include "util/bytes.h"

class Context
{
public:
  Context(Storage& _store);

  void call(Buffer& in_buf, Buffer* out_buf = nullptr);

  void reset();
  void flush();

  template <typename T>
  T& get(const Address& key)
  {
    if (auto ptr = get_contract_ptr(key); ptr != nullptr)
      return *(ptr->as<T>());
    throw Error("Address {} does not exist", key);
  }

  template <typename T, typename... Args>
  T& create(const Address& key, Args&&... args)
  {
    auto raw_data = store.get(sha256(key));
    if (raw_data)
      throw Error("Contract has existed already.");

    auto uniq = std::make_unique<T>(key);
    auto raw = uniq.get();
    cache[key] = std::move(uniq);

    raw->init(std::forward<Args>(args)...);
    raw->debug_create();
    return *raw;
  }

  static inline auto log = logger::get("context");

public:
  Storage& store;

private:
  Contract* get_contract_ptr(const Address& key);

private:
  std::unordered_map<Address, std::unique_ptr<Contract>> cache;
};
