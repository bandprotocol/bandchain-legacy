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
#include "store/context.h"
#include "store/global.h"

Account::Account(const Address& address)
    : Contract(address, ContractID::Account)
{
}

void Account::init(const VerifyKey& verify_key)
{
  m_verify_key = verify_key;
  m_nonce = 0;
}

Buffer Account::delegate_call(Buffer buf)
{
  auto sig = buf.read<Signature>();
  if (!ed25519_verify(sig, +m_verify_key, buf.as_span()))
    throw Error("Invalid Ed25519 signature");

  uint64_t new_nonce = buf.read<uint64_t>();
  if (+m_nonce >= new_nonce)
    throw Error("Invalid nonce");
  m_nonce = new_nonce;

  set_sender();

  Buffer ret;
  Global::get().m_ctx->call(buf, &ret);
  return ret;
}

uint256_t Account::get_nonce() const
{
  return +m_nonce;
}
