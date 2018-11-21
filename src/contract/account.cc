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

#include "account.h"

#include "crypto/ed25519.h"

void Account::init(const PublicKey& _publicKey)
{
  publicKey = _publicKey;
  nonce = 0;
}

void Account::setNonce(uint64_t _nonce)
{
  if (_nonce != +nonce + 1)
    throw Error("Account::setNonce: invalid nonce {} != {} + 1", _nonce,
                +nonce);
  nonce = _nonce;
}

void Account::verifySignature(gsl::span<const byte> data,
                              const Signature& sig) const
{
  if (!ed25519_verify(sig, +publicKey, data))
    throw Error("Account::verifySignature: invalid transaction signature");
}
