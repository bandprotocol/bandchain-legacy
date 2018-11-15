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

#include "context.h"

#include "contract/account.h"
#include "contract/creator.h"
#include "contract/governance.h"
#include "contract/stake.h"
#include "contract/tcr.h"
#include "contract/token.h"
#include "contract/voting.h"
#include "contract/wiki.h"
#include "crypto/sha256.h"
#include "store/global.h"

Context::Context(Storage& _store, GraphStore& _graph)
    : store(_store)
    , graph(_graph)
{
}

void Context::call(Buffer& in_buf, Buffer* out_buf)
{
  Address addr = in_buf.read<Address>();
  Contract* contract = get_contract_ptr(addr);

  if (contract == nullptr)
    throw Error("Contract not found");

  contract->call_buf(in_buf, out_buf);
}

Contract* Context::get_contract_ptr(const Address& key)
{
  if (auto it = cache.find(key); it != cache.end())
    return it->second.get();

  auto raw_data = store.get(sha256(key));
  if (!raw_data)
    return nullptr;

  Buffer buf{gsl::as_bytes(gsl::make_span(*raw_data))};
  ContractID contract_id(ContractID::Creator);
  buf >> contract_id;

  switch (contract_id) {
    case +ContractID::Creator:
      cache.emplace(key, std::make_unique<Creator>(key));
      break;
    case +ContractID::Account:
      cache.emplace(key, std::make_unique<Account>(key));
      break;
    case +ContractID::Token:
      cache.emplace(key, std::make_unique<Token>(key));
      break;
    case +ContractID::Voting:
      cache.emplace(key, std::make_unique<Voting>(key));
      break;
    case +ContractID::Registry:
      cache.emplace(key, std::make_unique<Registry>(key));
      break;
    case +ContractID::Stake:
      cache.emplace(key, std::make_unique<Stake>(key));
      break;
    case +ContractID::Governance:
      cache.emplace(key, std::make_unique<Governance>(key));
      break;
    case +ContractID::Wiki:
      cache.emplace(key, std::make_unique<Wiki>(key));
      break;
    default:
      throw Error("Contract id {} doesn't support.", contract_id);
  }

  return cache.at(key).get();
}

void Context::reset()
{
  cache.clear();
  Global::get().reset_per_tx();
}

void Context::flush()
{
  Global::get().flush = true;

  for (auto& p : cache) {
    p.second->debug_save();
  }
  cache.clear();
  Global::get().reset_per_tx();

  DEBUG(log, "============================================================"
             "============================================================");
}
