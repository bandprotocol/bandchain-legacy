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

#include <sodium/crypto_sign_ed25519.h>

#include "crypto/sha256.h"
#include "inc/essential.h"
#include "util/bytes.h"

/// Generate a random Ed25519 keypair
inline std::pair<VerifyKey, SecretKey> ed25519_generate_keypair()
{
  std::pair<VerifyKey, SecretKey> keypair;
  crypto_sign_ed25519_keypair(
      (unsigned char*)keypair.first.as_const_span().data(),
      (unsigned char*)keypair.second.as_const_span().data());
  return keypair;
}

/// Convert Ed25519 secret key to Ed25519 verify key
inline VerifyKey ed25519_sk_to_vk(const SecretKey& secret_key)
{
  VerifyKey verify_key;
  crypto_sign_ed25519_sk_to_pk(
      (unsigned char*)verify_key.as_const_span().data(),
      (unsigned char*)secret_key.as_const_span().data());
  return verify_key;
}

/// Convert Ed25519 verify key to address (unique to BAND).
inline Address ed25519_vk_to_addr(const VerifyKey& verify_key)
{
  Hash hash = sha256(verify_key);
  Address ret;
  std::memcpy(ret.as_span().data(), hash.as_const_span().data(), Address::Size);
  return ret;
}

/// Sign Ed25519 signature.
template <typename T>
inline Signature ed25519_sign(const SecretKey& secret_key, gsl::span<T> data)
{
  SecretKey sig;
  crypto_sign_ed25519_detached(
      (unsigned char*)sig.as_span().data(), nullptr,
      (unsigned char*)data.data(), data.size_bytes(),
      (unsigned char*)secret_key.as_const_span().data());
  return sig;
}

/// Verify Ed25519 signature.
template <typename T>
inline bool ed25519_verify(const Signature& sig, const VerifyKey& verify_key,
                           gsl::span<T> data)
{
  return crypto_sign_ed25519_verify_detached(
             (unsigned char*)sig.as_const_span().data(),
             (unsigned char*)data.data(), data.size_bytes(),
             (unsigned char*)verify_key.as_const_span().data()) == 0;
  return false;
}
