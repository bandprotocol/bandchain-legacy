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
#include "store/context.h"
#include "util/bytes.h"

class Global
{
public:
  Context* m_ctx = nullptr;
  Address sender{};
  uint64_t block_time{};
  Address block_proposer{};
  Hash tx_hash{};
  bool flush = false;

  static Global& get()
  {
    if (!global)
      global = std::make_unique<Global>();
    return *global;
  }

  void reset_per_tx()
  {
    sender = Address();
    tx_hash = Hash();
    flush = false;
  }

private:
  inline static std::unique_ptr<Global> global;
};
