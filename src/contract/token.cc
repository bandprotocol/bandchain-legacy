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

#include "token.h"

void Token::init(const Ident& _baseIdent, const Curve& _curveData)
{
  // Verify that base token contract exists.
  storage.load<Token>(_baseIdent);

  curveData = _curveData;
  baseIdent = _baseIdent;
  currentSupply = 0;
}

Ident Token::base() const
{
  return +baseIdent;
}

void Token::mint(const Ident& receiver, const uint256_t& value)
{
  balances[receiver] = +balances[receiver] + value;
  currentSupply = +currentSupply + value;
}

void Token::transfer(const Ident& src, const Ident& dst, const uint256_t& value)
{
  balances[src] = +balances[src] - value;
  balances[dst] = +balances[dst] + value;
}

uint256_t Token::buy(const Ident& buyer, const uint256_t& value)
{
  Curve curve = +curveData;
  auto& baseToken = storage.load<Token>(+baseIdent);

  uint256_t buyTokens =
      curve.apply(+currentSupply + value) - curve.apply(+currentSupply);

  baseToken.balances[buyer] = +baseToken.balances[buyer] - buyTokens;
  balances[buyer] = +balances[buyer] + value;

  currentSupply = +currentSupply + value;
  return buyTokens;
}

uint256_t Token::sell(const Ident& seller, const uint256_t& value)
{
  Curve curve = +curveData;
  auto& baseToken = storage.load<Token>(+baseIdent);

  uint256_t sellTokens =
      curve.apply(+currentSupply) - curve.apply(+currentSupply - value);

  baseToken.balances[seller] = +baseToken.balances[seller] + sellTokens;
  balances[seller] = +balances[seller] + value;

  currentSupply = +currentSupply - value;
  return sellTokens;
}

// void Token::mint(uint256_t value)
// {
//   // TODO
//   m_balances[get_sender()] = +m_balances[get_sender()] + value;
//   current_supply = +current_supply + value;
// }

// void Token::transfer(Address dest, uint256_t value)
// {
//   m_balances[get_sender()] = +m_balances[get_sender()] - value;
//   m_balances[dest] = +m_balances[dest] + value;
// }

// void Token::buy(uint256_t value)
// {
//   Curve real_curve = +buy_curve;
//   uint256_t buy_price = real_curve.apply(+current_supply + value) -
//                         real_curve.apply(+current_supply);

//   auto& base_contract = Global::get().m_ctx->get<Token>(+base_token_id);

//   base_contract.m_balances[get_sender()] =
//       +base_contract.m_balances[get_sender()] - buy_price;
//   m_balances[get_sender()] = +m_balances[get_sender()] + value;
//   current_supply = +current_supply + value;
// }

// void Token::sell(uint256_t value)
// {
//   Curve real_curve = +buy_curve;
//   uint256_t sell_price = real_curve.apply(+current_supply) -
//                          real_curve.apply(+current_supply - value);

//   auto& base_contract = Global::get().m_ctx->get<Token>(+base_token_id);

//   base_contract.m_balances[get_sender()] =
//       +base_contract.m_balances[get_sender()] + sell_price;
//   m_balances[get_sender()] = +m_balances[get_sender()] - value;
//   current_supply = +current_supply - value;
// }

// uint256_t Token::balance(Address address) const
// {
//   return +m_balances[address];
// }

// uint256_t Token::spot_price() const
// {
//   Curve real_curve = +buy_curve;
//   return real_curve.apply(+current_supply + 1) -
//          real_curve.apply(+current_supply);
// }

// uint256_t Token::bulk_price(uint256_t value) const
// {
//   Curve real_curve = +buy_curve;
//   return real_curve.apply(+current_supply + value) -
//          real_curve.apply(+current_supply);
// }

// uint256_t Token::get_current_supply() const
// {
//   return +current_supply;
// }

// void Token::debug_create() const
// {
//   DEBUG(log, "token created at {} {}", m_addr, (void*)this);
//   DEBUG(log, "Base token id: {}", +base_token_id);
//   DEBUG(log, "Buy curve: {}", +buy_curve);
// }

// void Token::debug_save() const
// {
//   DEBUG(log, "token saved at {} {}", m_addr, (void*)this);
//   DEBUG(log, "Supply {}", +current_supply);
// }
