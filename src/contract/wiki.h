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

#include "inc/essential.h"
#include "store/contract.h"
#include "util/json.h"

class Wiki : public Contract
{
public:
  Wiki(const Address& wiki_id);

  void init() {}

  // Callable

  // Inner function for first phase add 1 quad
  void
  add_triple(std::string subject, std::string predicate, std::string object);

  void
  delete_triple(std::string subject, std::string predicate, std::string object);

  void debug_create() const final;
  void debug_save() const final;

private:
  static inline auto log = logger::get("wiki");
};
