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

void PrimaryListener::switchMode(PrimaryMode mode)
{
  // TODO
}

void PrimaryListener::validateTransaction(const Ident& user,
                                          const Signature& sig, uint64_t nonce,
                                          gsl::span<const byte> data)
{
  // TODO
}

void PrimaryListener::handleCreateAccount(const CreateAccountMsg& msg)
{
  // TODO
}

void PrimaryListener::handleCreateToken(const CreateTokenMsg& msg)
{
  // TODO
}

void PrimaryListener::handleMintToken(const MintTokenMsg& msg)
{
  // TODO
}

void PrimaryListener::handleTransferToken(const TransferTokenMsg& msg)
{
  // TODO
}

void PrimaryListener::handleBuyToken(const BuyTokenMsg& msg)
{
  // TODO
}

void PrimaryListener::handleSellToken(const SellTokenMsg& msg)
{
  // TODO
}
