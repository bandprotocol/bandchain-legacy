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

#include "session.h"

Session::Session(boost::asio::ip::tcp::socket _socket, NetApplication& _app)
    : app(_app)
    , socket(std::move(_socket))
{
}

void Session::serve()
{
  async_read();
}

void Session::process_read(size_t length)
{
  read_buffer << gsl::span(socket_buffer, length);
  while (app.process(read_buffer, write_buffer)) {
    // Keep processing incoming messages until everything is done.
  }
  async_write();
}

void Session::process_write(size_t length)
{
  write_buffer.consume(length);
  async_read();
}

void Session::async_read()
{
  boost::asio::async_read(
      socket, boost::asio::buffer(socket_buffer, sizeof(socket_buffer)),
      boost::asio::transfer_at_least(1),
      [self = shared_from_this()](const auto ec, size_t length) {
        if (!ec) {
          self->process_read(length);
        } else {
          WARN(log, "Failed to read from socket. Client disconnected");
        }
      });
}

void Session::async_write()
{
  if (write_buffer.empty()) {
    return async_read();
  }

  auto write_span = write_buffer.as_span();
  boost::asio::async_write(
      socket, boost::asio::buffer(write_span.data(), write_span.size_bytes()),
      [self = shared_from_this()](const auto ec, size_t length) {
        if (!ec) {
          self->process_write(length);
        } else {
          WARN(log, "Failed to write to socket. Client disconnected");
        }
      });
}
