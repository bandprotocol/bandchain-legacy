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
#include "store/contract.h"
#include "store/data.h"
#include "util/bytes.h"

class Account final : public Contract
{
public:
  Account(const Address& address);

  void init(const VerifyKey& verify_key);

  Buffer delegate_call(Buffer buf);

  uint256_t get_nonce() const;

  VerifyKey get_vk() const;
  void debug_create() const final
  {
    DEBUG(log, "account created at {} nonce = {} {}", m_addr, +m_nonce,
          (void*)this);
  }

  void debug_save() const final
  {
    DEBUG(log, "account saved at {} nonce = {} {}", m_addr, +m_nonce,
          (void*)this);
  }

private:
  Data<VerifyKey> m_verify_key{sha256(m_addr, uint16_t(1))};
  Data<uint64_t> m_nonce{sha256(m_addr, uint16_t(2))};

  static inline auto log = logger::get("account");
};
