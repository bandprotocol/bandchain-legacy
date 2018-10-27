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

#include "inc/essential.h"
#include "net/server.h"

class PingPongApplication : public NetApplication
{
private:
  bool process(Buffer& read_buffer, Buffer& write_buffer) final
  {
    write_buffer << read_buffer.as_span();
    read_buffer.clear();
    return false;
  }
};

int main()
{
  PingPongApplication app;
  boost::asio::io_service service;

  Server server(service, app, 46658);
  server.start();

  service.run();
  return 0;
}
