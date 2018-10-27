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
#include "store/data.h"
#include "util/buffer.h"

class Upgradable
{
public:
  virtual ~Upgradable()
  {
  }
  virtual Address get_governance_id() const = 0;
  virtual void upgrade(Buffer buf) = 0;
  virtual std::string to_string(Buffer buf) const = 0;
};

template <typename T>
class UpgradableContract : public Contract, public Upgradable
{
public:
  UpgradableContract(const Address& address, ContractID _contract_id)
      : Contract(address, _contract_id)
  {
  }

  Address get_governance_id() const final
  {
    return +governance_id;
  }
  void upgrade(Buffer buf) final
  {
    params.upgrade(buf);
  }
  std::string to_string(Buffer buf) const final
  {
    return params.parse_buffer(buf);
  }

protected:
  T params{sha256(m_addr, uint16_t(0), uint16_t(1))};
  Data<Address> governance_id{sha256(m_addr, uint16_t(0), uint16_t(2))};
};
