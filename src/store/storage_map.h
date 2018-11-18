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

#include "store/storage.h"

class StorageMap : public Storage
{
public:
  void put(const Hash& key, const std::string& val) final;
  nonstd::optional<std::string> get(const Hash& key) const final;
  void del(const Hash& key) final;

  void commit_block() final {}

  // TODO
  void switch_to_tx() final
  {
    NOCOMMIT_LOG("Now apply mode");
  }
  void switch_to_check() final
  {
    NOCOMMIT_LOG("Now check mode");
  }
  void switch_to_query() final
  {
    NOCOMMIT_LOG("Now query mode");
  }

  void save_protected_key(const std::string& key, const std::string& val) final;
  nonstd::optional<std::string> get_protected_key(const std::string& key) final;

public:
  std::unordered_map<Hash, std::string> data;
};
