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
#include "util/buffer.h"

class Session : public std::enable_shared_from_this<Session>
{
public:
  Session(boost::asio::ip::tcp::socket socket, NetApplication& app);

  void serve();

private:
  void process_read(size_t length);
  void process_write(size_t length);

  void async_read();
  void async_write();

private:
  NetApplication& app;
  boost::asio::ip::tcp::socket socket;

  Buffer read_buffer;
  Buffer write_buffer;

  std::byte socket_buffer[1024];

  static inline auto log = logger::get("session");
};
