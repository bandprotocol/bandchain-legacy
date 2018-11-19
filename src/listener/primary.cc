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

#include "primary.h"

#include "store/storage.h"

PrimaryListener::PrimaryListener(Storage& _store)
    : store(_store)
{
}

void PrimaryListener::switchMode(PrimaryMode mode)
{
  // TODO
}

void PrimaryListener::load()
{
  // TODO
}

void PrimaryListener::begin(const BlockMsg& blk)
{
  // TODO
}

void PrimaryListener::commit(const BlockMsg& blk)
{
  // TODO
}

void PrimaryListener::validateTransaction(const HeaderMsg& hdr,
                                          gsl::span<const byte> data)
{
  // TODO
}

CreateAccountResponse PrimaryListener::handleCreateAccount(
    const BlockMsg& blk, const HeaderMsg& hdr, const CreateAccountMsg& msg)
{
  // TODO
  return {};
}

CreateTokenResponse PrimaryListener::handleCreateToken(
    const BlockMsg& blk, const HeaderMsg& hdr, const CreateTokenMsg& msg)
{
  // TODO
  return {};
}

MintTokenResponse PrimaryListener::handleMintToken(const BlockMsg& blk,
                                                   const HeaderMsg& hdr,
                                                   const MintTokenMsg& msg)
{
  // TODO
  return {};
}

TransferTokenResponse PrimaryListener::handleTransferToken(
    const BlockMsg& blk, const HeaderMsg& hdr, const TransferTokenMsg& msg)
{
  // TODO
  return {};
}

BuyTokenResponse PrimaryListener::handleBuyToken(const BlockMsg& blk,
                                                 const HeaderMsg& hdr,
                                                 const BuyTokenMsg& msg)
{
  // TODO
  return {};
}

SellTokenResponse PrimaryListener::handleSellToken(const BlockMsg& blk,
                                                   const HeaderMsg& hdr,
                                                   const SellTokenMsg& msg)
{
  // TODO
  return {};
}
