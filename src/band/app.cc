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
#include "contract/token.h"
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
  auto result = ctx.store.get_protected_key("Band Protocol Block Height");
  if (result) {
    last_block_height = Buffer::deserialize<uint64_t>(*result);
  } else {
    last_block_height = 0;
  }
}

std::string BandApplication::get_current_app_hash() const
{
  // TODO
  return "";
}

void BandApplication::init(const std::string& init_state)
{
  ctx.create<Creator>(Address{});
  Address band = Address::from_hex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
  Curve linear = Curve(std::make_unique<EqVar>());
  ctx.create<Token>(band, band, linear);
  ctx.flush();
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

  Buffer msg_buf(gsl::make_span(data));
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
  Buffer msg_buf(gsl::make_span(msg_raw));
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
