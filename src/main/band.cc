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

#include "band/app.h"
#include "inc/essential.h"
#include "net/server.h"
#include "store/context.h"
#include "store/storage_map.h"
#include "store/storage_rocksdb.h"
#include "util/cli.h"

CmdArg<bool> use_db("use-db", "set this flag to use rocksdb");

int main(int argc, char* argv[])
{
  Cmd cmd(argc, argv);
  LOG("BAND blockchain appliation starting...");

  std::unique_ptr<Storage> store;
  if (+use_db) {
    store = std::make_unique<StorageDB>();
  } else {
    store = std::make_unique<StorageMap>();
  }
  Context ctx(*store);
  BandApplication app(ctx);
  boost::asio::io_service service;

  Server server(service, app, 26658);
  server.start();

  service.run();
  return 0;
}
