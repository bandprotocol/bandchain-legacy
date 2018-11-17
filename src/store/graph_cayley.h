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
#include "store/graph.h"

class GraphStoreCayley : public GraphStore
{
public:
  GraphStoreCayley(boost::asio::io_service& service,
                   const std::string& _hostname,
                   uint16_t _port);

  void add_edge(const std::string& subject,
                const std::string& predicate,
                const std::string& object,
                const std::string& label) final;
  void delete_edge(const std::string& subject,
                   const std::string& predicate,
                   const std::string& object,
                   const std::string& label) final;

private:
  std::string create_body_msg(const std::string& subject,
                              const std::string& predicate,
                              const std::string& object,
                              const std::string& label);

  void send_request_and_get_response(const std::string& path,
                                     const std::string& data);

private:
  std::string hostname;
  uint16_t port;
  boost::asio::io_service& service;
};
