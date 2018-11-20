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

#include "contract/account.h"
#include "contract/token.h"

PrimaryListener::PrimaryListener(Storage& _storage)
    : storage(_storage)
{
}

void PrimaryListener::load()
{
  // TODO
}

void PrimaryListener::init(const GenesisMsg& genesis)
{
  storage.switchToApply();

  // Set up the genesis account. Owner of this account can use it to create more
  // accounts to this blockchain later.
  storage.create<Account>(genesis.account.to_string(), genesis.publicKey);

  storage.commit();
}

void PrimaryListener::begin(const BlockMsg& blk)
{
  storage.reset();
}

void PrimaryListener::commit(const BlockMsg& blk)
{
  storage.commit();
}

void PrimaryListener::validateTransaction(PrimaryMode mode,
                                          const HeaderMsg& hdr,
                                          gsl::span<const byte> data)
{
  switch (mode) {
    case +PrimaryMode::Check:
      storage.switchToCheck();
      break;
    case +PrimaryMode::Apply:
      storage.switchToApply();
      break;
  }

  auto& account = storage.load<Account>(hdr.user.to_string());
  account.verifySignature(data, hdr.sig);
  account.setNonce(hdr.nonce);
}

CreateAccountResponse PrimaryListener::handle(const BlockMsg& blk,
                                              const HeaderMsg& hdr,
                                              const CreateAccountMsg& msg)
{
  // TODO:
  storage.create<Account>(msg.user.to_string(), msg.pk);

  return {};
}

CreateTokenResponse PrimaryListener::handle(const BlockMsg& blk,
                                            const HeaderMsg& hdr,
                                            const CreateTokenMsg& msg)
{
  // TODO
  return {};
}

MintTokenResponse PrimaryListener::handle(const BlockMsg& blk,
                                          const HeaderMsg& hdr,
                                          const MintTokenMsg& msg)
{
  // TODO
  return {};
}

TransferTokenResponse PrimaryListener::handle(const BlockMsg& blk,
                                              const HeaderMsg& hdr,
                                              const TransferTokenMsg& msg)
{
  // TODO
  return {};
}

BuyTokenResponse PrimaryListener::handle(const BlockMsg& blk,
                                         const HeaderMsg& hdr,
                                         const BuyTokenMsg& msg)
{
  // TODO
  return {};
}

SellTokenResponse PrimaryListener::handle(const BlockMsg& blk,
                                          const HeaderMsg& hdr,
                                          const SellTokenMsg& msg)
{
  // TODO
  return {};
}
