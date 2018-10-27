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

#include "server.h"

#include "net/session.h"

using boost::asio::ip::tcp;

Server::Server(boost::asio::io_service& _service, NetApplication& _app,
               uint16_t port)
    : app(_app)
    , service(_service)
    , acceptor(service, tcp::endpoint(tcp::v4(), port))
    , socket(service)
{
}

void Server::start()
{
  accept_connection();
}

void Server::accept_connection()
{
  acceptor.async_accept(socket, [this](const auto error_code) {
    if (!error_code) {
      std::make_shared<Session>(std::move(socket), app)->serve();
      accept_connection();
    } else {
      throw Failure("Failed to accept socket connection");
    }
  });
}
