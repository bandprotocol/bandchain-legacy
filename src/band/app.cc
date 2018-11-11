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

#include "app.h"

#include <boost/scope_exit.hpp>

#include "contract/account.h"
#include "contract/creator.h"
#include "contract/stake.h"
#include "contract/token.h"
#include "crypto/ed25519.h"
#include "crypto/sha256.h"
#include "store/contract.h"
#include "store/global.h"
#include "util/cli.h"
#include "util/equation.h"

CmdArg<bool> fake_time("fake_time", "use tx time as global time", 'f');

BandApplication::BandApplication(Context& _ctx)
    : ctx(_ctx)
    , use_fake_time(+fake_time)
{
  Global::get().m_ctx = &ctx;

  // get block height from storage
  auto height_raw = ctx.store.get_protected_key("Band Protocol Block Height");
  if (height_raw) {
    last_block_height = Buffer::deserialize<uint64_t>(*height_raw);
  } else {
    last_block_height = 0;
  }

  auto vector_raw = ctx.store.get_protected_key("Validator Set");
  if (vector_raw) {
    validators =
        Buffer::deserialize<std::vector<std::pair<VerifyKey, uint64_t>>>(
            *vector_raw);
  } else {
    validators = std::vector<std::pair<VerifyKey, uint64_t>>{};
  }
}

std::string BandApplication::get_current_app_hash() const
{
  // TODO
  return "";
}

void BandApplication::init(
    const std::vector<std::pair<VerifyKey, uint64_t>>& _validators,
    const std::string& init_state)
{
  ctx.create<Creator>(Address{});
  ctx.store.switch_to_tx();
  Address band = Address::hex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
  Curve linear = Curve(std::make_unique<EqVar>());
  ctx.create<Token>(band, band, linear);
  Address stake_id = Address::hex("1313131313131313131313131313131313131313");
  Stake& stake = ctx.create<Stake>(stake_id, band);

  validators = _validators;
  number_validators = validators.size();

  Token& band_token = ctx.get<Token>(band);
  for (auto& [vk, power] : validators) {
    // for now mint token to new proposer for create party
    Address validator_address = ed25519_vk_to_addr(vk);
    Account& account = ctx.create<Account>(validator_address, vk);
    account.set_sender();
    band_token.mint(power);

    // Default party share all reward to every delegated
    stake.create_party(power, 0, 1);
  }
  ctx.flush();
  ctx.store.save_protected_key(
      "Validator Set",
      Buffer::serialize<std::vector<std::pair<VerifyKey, uint64_t>>>(
          validators));
  ctx.store.commit_block();
}

std::string BandApplication::query(const std::string& path,
                                   const std::string& data)
{
  BOOST_SCOPE_EXIT(&ctx)
  {
    ctx.reset();
  }
  BOOST_SCOPE_EXIT_END

  Global::get().m_ctx->store.switch_to_query();

  if (path == "abi") {
    return Contract::get_abi_interface().dump(4);
  }

  Buffer msg_buf(gsl::as_bytes(gsl::make_span(data)));
  uint64_t ts = msg_buf.read<uint64_t>();
  (void)ts;

  Buffer result;
  ctx.call(msg_buf, &result);

  return result.to_raw_string();
}

void BandApplication::check(const std::string& msg_raw)
{
  // TODO
}

std::string BandApplication::apply(const std::string& msg_raw)
{
  BOOST_SCOPE_EXIT(&ctx)
  {
    ctx.reset();
  }
  BOOST_SCOPE_EXIT_END

  Global::get().m_ctx->store.switch_to_tx();
  Buffer msg_buf(gsl::as_bytes(gsl::make_span(msg_raw)));
  Global::get().tx_hash = sha256(gsl::make_span(msg_raw));
  uint64_t ts = msg_buf.read<uint64_t>();
  // Mock
  if (use_fake_time)
    Global::get().block_time = ts;
  // (void)ts;

  Buffer result;
  ctx.call(msg_buf, &result);
  ctx.flush();

  return result.to_raw_string();
}

void BandApplication::begin_block(uint64_t block_time,
                                  const Address& block_proposer)
{
  Global::get().block_time = block_time;
  Global::get().block_proposer = block_proposer;

  // Add reward to proposer
  Address stake_id = Address::hex("1313131313131313131313131313131313131313");
  ctx.store.switch_to_tx();
  Stake& stake = ctx.get<Stake>(stake_id);
  stake.add_reward(block_proposer, 100);
  // NOCOMMIT_LOG("{} propose by {}", last_block_height + 1, block_proposer);
  ctx.flush();
}

std::vector<std::pair<VerifyKey, uint64_t>> BandApplication::end_block()
{
  Address stake_id = Address::hex("1313131313131313131313131313131313131313");
  Stake& stake = ctx.get<Stake>(stake_id);

  std::vector<std::pair<VerifyKey, uint64_t>> new_validators;

  auto topx = stake.topx(number_validators);
  for (auto& [addr, power] : topx) {
    // Voting power set to be 1 for each validator.
    new_validators.emplace_back(ctx.get<Account>(addr).get_vk(), 1);
  }

  std::vector<std::pair<VerifyKey, uint64_t>> updated_validators;

  for (auto& old_validator : validators) {
    bool found = false;
    for (auto& new_validator : new_validators) {
      if (new_validator.first == old_validator.first) {
        found = true;
        break;
      }
    }
    if (!found) {
      // Remove old validator
      updated_validators.emplace_back(old_validator.first, 0);
    }
  }

  for (auto& new_validator : new_validators) {
    bool found = false;
    for (auto& old : validators) {
      if (new_validator.first == old.first) {
        found = true;
        break;
      }
    }
    if (!found) {
      // Add new validator
      updated_validators.push_back(new_validator);
    }
  }

  ctx.store.save_protected_key("Band Protocol Block Height",
                               Buffer::serialize<uint64_t>(last_block_height));

  // TODO: get updated validator vector (need to compare with old validator)
  // validators vs new_validators
  Global::get().m_ctx->store.save_protected_key(
      "Validator Set",
      Buffer::serialize<std::vector<std::pair<VerifyKey, uint64_t>>>(
          new_validators));

  validators = new_validators;
  return updated_validators;
}

void BandApplication::commit_block()
{
  Global::get().m_ctx->store.commit_block();
}
