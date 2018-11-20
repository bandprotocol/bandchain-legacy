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

#include "listener/logging.h"
#include "listener/manager.h"
#include "listener/primary.h"
#include "net/server.h"
#include "net/tmapp.h"
#include "store/storage.h"
#include "store/storage_map.h"

class BandLoggingApplication : public TendermintApplication
{
public:
  BandLoggingApplication(ListenerManager& _manager)
      : manager(_manager)
  {
  }

  std::string get_name() const final
  {
    return "band-logging";
  }

  std::string get_version() const final
  {
    return "n/a";
  }

  std::string get_current_app_hash() const final
  {
    return "";
  }

  void init(const std::vector<std::pair<VerifyKey, uint64_t>>& _validators,
            const std::string& init_state) final
  {
    NOCOMMIT_LOG("INIT STATE IS {}", init_state);
  }

  std::string query(const std::string& path, const std::string& data) final
  {
    return "";
  }

  void check(const std::string& msg_raw) final
  {
    manager.checkTransaction(gsl::as_bytes(gsl::make_span(msg_raw)));
  }

  std::string apply(const std::string& msg_raw) final
  {
    manager.applyTransaction(gsl::as_bytes(gsl::make_span(msg_raw)));
    return "";
  }

  void begin_block(uint64_t block_time, const Address& block_proposer) final
  {
    manager.beginBlock(block_time, block_proposer);
  }

  std::vector<std::pair<VerifyKey, uint64_t>> end_block() final
  {
    return {};
  }

  void commit_block() final {}

private:
  ListenerManager& manager;
};

int main(int argc, char* argv[])
{
  boost::asio::io_service service;

  ListenerManager manager;
  StorageMap storage;

  manager.setPrimary(std::make_unique<PrimaryListener>(storage));
  manager.addListener(std::make_unique<LoggingListener>());

  BandLoggingApplication app(manager);

  Server server(service, app, 26658);
  server.start();

  service.run();
  return 0;
}
