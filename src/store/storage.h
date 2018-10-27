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
#include "util/bytes.h"

class Storage
{
public:
  virtual ~Storage()
  {
  }

  // Save or create value to key.
  virtual void put(const Hash& key, const std::string& val) = 0;

  // Get value from key or return nullopt if not exist.
  virtual nonstd::optional<std::string> get(const Hash& key) const = 0;

  // Delete key from storage.
  virtual void del(const Hash& key) = 0;

  // Commit tx_transaction
  virtual void commit_block() = 0;

  virtual void switch_to_tx() = 0;
  virtual void switch_to_check() = 0;
  virtual void switch_to_query() = 0;

  virtual void save_protected_key(const std::string& key,
                                  const std::string& val) = 0;

  virtual nonstd::optional<std::string>
  get_protected_key(const std::string& key) = 0;
};
