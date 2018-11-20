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
#include "store/data_map.h"
#include "util/bytes.h"
#include "util/equation.h"

// Token contract encapsulates the management of tokens in Band blockchain. Each
// token contract keeps track of its token internal information, including
// balances of each owners, the bonding curve, the base token, the current
// supply, etc.
class Token final : public Contract
{
public:
  using Contract::Contract;

  /// All token keys must begin with "t/" namespace.
  static constexpr char KeyPrefix[] = "t/";

  void init(const Ident& baseToken, const Curve& curve);
  void mint(const Ident& receiver, const uint256_t& value);
  void transfer(const Ident& src, const Ident& dst, const uint256_t& value);
  void buy(const Ident& buyer, const uint256_t& value);
  void sell(const Ident& seller, const uint256_t& value);

private:
  DATA(Curve, curveData)
  DATA(Ident, baseTokenIdent)
  DATA(uint256_t, currentSupply)

  DATAMAP(Ident, Data<uint256_t>, balances)
};

// {
// public:
//   Token(const Address& token_id);

//   void init(const Address& _base_token_id, const Curve& _buy_curve);

//   void mint(uint256_t value);

//   void transfer(Address dest, uint256_t value);

//   void buy(uint256_t value);

//   void sell(uint256_t value);

//   uint256_t balance(Address address) const;

//   uint256_t spot_price() const;

//   uint256_t bulk_price(uint256_t value) const;

//   uint256_t get_current_supply() const;

// public:
//   void debug_create() const final;
//   void debug_save() const final;

//   Data<Address> base_token_id{sha256(m_addr, uint16_t(1))};
//   Data<Curve> buy_curve{sha256(m_addr, uint16_t(2))};

// private:
//   Mapping<Address, Data<uint256_t>> m_balances{sha256(m_addr,
//   uint16_t(3))}; Data<uint256_t> current_supply{sha256(m_addr,
//   uint16_t(4))};

//   static inline auto log = logger::get("token");
// };
