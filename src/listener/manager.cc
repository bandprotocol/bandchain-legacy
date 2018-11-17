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

#include "manager.h"

#include "util/buffer.h"

namespace
{
/// Parser the header part of the given raw messages. This helper function is
/// used both during checkTx and applyTx
std::tuple<Ident, Signature, uint64_t, gsl::span<const byte>, Buffer>
parseHeader(gsl::span<const byte> raw)
{
  Buffer buf(raw);

  auto user = buf.read<Ident>();
  auto sig = buf.read<Signature>();
  auto nonce = buf.read<uint64_t>();

  return {
      user, sig, nonce, raw.last(raw.size() - Ident::Size - Signature::Size),
      buf,
  };
}
} // namespace

void ListenerManager::loadStates()
{
  if (primary)
    primary->load();

  for (auto& listener : listeners)
    listener->load();
}

void ListenerManager::beginBlock(uint64_t timestamp, const Address& proposer)
{
  if (primary)
    primary->begin(timestamp, proposer);

  for (auto& listener : listeners)
    listener->begin(timestamp, proposer);
}

void ListenerManager::checkTransaction(gsl::span<const byte> raw)
{
  if (!primary)
    throw Failure("endBlock: called without primary listener set");

  auto [user, sig, nonce, data, buf] = parseHeader(raw);

  (void)buf;

  primary->switchMode(PrimaryMode::Check);
  primary->validateTransaction(user, sig, nonce, data);
  primary->switchMode(PrimaryMode::None);
}

void ListenerManager::applyTransaction(gsl::span<const byte> raw)
{
  auto [user, sig, nonce, data, buf] = parseHeader(raw);
  auto msgType = buf.read<MsgType>();

  if (primary) {
    primary->switchMode(PrimaryMode::Apply);
    primary->validateTransaction(user, sig, nonce, data);
  }

  switch (msgType) {
#define HANDLE_APPLY_CASE(R, _, MSG)                                           \
  case +MsgType::MSG: {                                                        \
    auto msg = buf.read<BAND_MACRO_MSG(MSG)>();                                \
    primary->BAND_MACRO_HANDLE(MSG)(msg);                                      \
    for (auto& listener : listeners) {                                         \
      listener->BAND_MACRO_HANDLE(MSG)(msg);                                   \
    }                                                                          \
  }

    BAND_MACRO_MESSAGE_FOR_EACH(HANDLE_APPLY_CASE)
#undef HANDLE_APPLY_CASE
  }

  if (primary) {
    primary->switchMode(PrimaryMode::None);
  }
}

std::vector<ValidatorUpdate> ListenerManager::endBlock()
{
  if (!primary)
    throw Failure("endBlock: called without primary listener set");

  // TODO
  return {};
}

void ListenerManager::commitBlock()
{
  if (primary)
    primary->commit();

  for (auto& listener : listeners)
    listener->commit();
}

void ListenerManager::setPrimary(std::unique_ptr<PrimaryListener> _primary)
{
  if (primary)
    throw Failure("setPrimary: set primary while one already exists");

  primary = std::move(_primary);
}

void ListenerManager::addListener(std::unique_ptr<BaseListener> listener)
{
  if (dynamic_cast<PrimaryListener*>(listener.get()) != nullptr)
    throw Failure("addListener: add primary listener into non-primary bucket");

  listeners.push_back(std::move(listener));
}
