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
#include "util/iban.h"

class Vars
{
public:
  virtual ~Vars() {}

  virtual uint256_t get_x() const = 0;
  virtual uint256_t get_external_price(const Address& key) const = 0;
  virtual uint256_t get_contract_price(const Address& key) const = 0;
};

class VarsSimple : public Vars
{
public:
  VarsSimple(uint256_t _s)
      : s(_s)
  {
  }

  uint256_t get_x() const final { return s; }
  uint256_t get_external_price(const Address& key) const
  {
    throw Error("External price isn't supported now");
  }
  uint256_t get_contract_price(const Address& key) const
  {
    throw Error("Contract price isn't supported now");
  }

  void set_s(const uint256_t& value) { s = value; }

private:
  uint256_t s;
};
