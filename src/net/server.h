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

#include <boost/asio.hpp>

#include "inc/essential.h"
#include "net/netapp.h"

class Server
{
public:
  Server(boost::asio::io_service& service, NetApplication& app, uint16_t port);

  /// Run the server and block forever.
  void start();

private:
  /// Add an asynchronous call to accept a new connection. Once a connection is
  /// accepted, another accept_connection will be invoked automatically.
  void accept_connection();

private:
  NetApplication& app;

  boost::asio::io_service& service;
  boost::asio::ip::tcp::acceptor acceptor;
  boost::asio::ip::tcp::socket socket;
};
