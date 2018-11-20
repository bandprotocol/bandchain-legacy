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
#include "util/bytes.h"

/// Account contract encapsulates the information stored for each of the account
/// on Band blockchain. It is responsible for verifying public/private key pair
/// signature and nonce.
class Account final : public Contract
{
public:
  using Contract::Contract;

  /// All account keys must begin with "u/" namespace.
  static constexpr char KeyPrefix[] = "u/";

  /// Initialize account information. To be called right after the creation.
  void init(const PublicKey& publicKey);

  /// Set the nonce of this account to the new value. Note that the new nonce
  /// must be 1 + the old nonce, or else this function will throw.
  void setNonce(uint64_t nonce);

  /// Check whether the given data and signature match with each other with
  /// respect to this account's public key.
  void verifySignature(gsl::span<const byte> data, const Signature& sig) const;

private:
  DATA(PublicKey, publicKey)
  DATA(uint256_t, nonce)
};
