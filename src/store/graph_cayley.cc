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

#include "store/graph_cayley.h"

#include "util/json.h"

using boost::asio::ip::tcp;

GraphStoreCayley::GraphStoreCayley(boost::asio::io_service& _service,
                                   const std::string& _hostname, uint16_t _port)
    : hostname(_hostname)
    , port(_port)
    , service(_service)
{
}

void GraphStoreCayley::add_edge(const std::string& subject,
                                const std::string& predicate,
                                const std::string& object,
                                const std::string& label)
{
  send_request_and_get_response(
      "/api/v2/write", create_body_msg(subject, predicate, object, label));
}

void GraphStoreCayley::delete_edge(const std::string& subject,
                                   const std::string& predicate,
                                   const std::string& object,
                                   const std::string& label)
{
  send_request_and_get_response(
      "/api/v2/delete", create_body_msg(subject, predicate, object, label));
}

std::string GraphStoreCayley::create_body_msg(const std::string& subject,
                                              const std::string& predicate,
                                              const std::string& object,
                                              const std::string& label)
{
  json j;
  j["subject"] = subject;
  j["predicate"] = predicate;
  j["object"] = object;
  j["label"] = label;
  return "[" + j.dump() + "]";
}

void GraphStoreCayley::send_request_and_get_response(const std::string& path,
                                                     const std::string& data)
{
  json result_json;
  try {
    tcp::resolver resolver(service);
    tcp::resolver::query query(hostname, std::to_string(port));
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    tcp::socket socket(service);
    boost::asio::connect(socket, endpoint_iterator);

    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    request_stream << "POST " << path << " HTTP/1.1\r\n";
    request_stream << "Host: " << hostname << ":" << port << "\r\n";
    request_stream << "Content-Type: application/json\r\n";
    request_stream << "accept: application/json\r\n";
    request_stream << "Content-Length: " << data.length() << "\r\n";
    request_stream << "Connection: close\r\n\r\n";
    request_stream << data;

    // Send the request.
    boost::asio::write(socket, request);

    // Read response
    boost::asio::streambuf response;
    boost::system::error_code ec;
    boost::asio::read(socket, response, ec);
    if (ec != boost::asio::error::eof)
      throw Failure("Failed in reading response");

    std::istream response_stream(&response);
    std::string http_version;
    response_stream >> http_version;
    uint16_t response_code;
    response_stream >> response_code;
    std::string status_message;
    std::getline(response_stream, status_message);
    if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
      throw Failure("Invalid response");
    }
    if (response_code != 200) {
      throw Failure("Response returned with status code "s +
                    std::to_string(response_code));
    }

    // Read other header
    std::string header;
    while (std::getline(response_stream, header) && header != "\r")
      ;

    // Read result
    std::string result;
    std::getline(response_stream, result);

    result_json = json::parse(result);

  } catch (std::exception& e) {
    throw Failure(e.what());
  }

  auto it = result_json.find("result");
  if (it == result_json.end()) {
    it = result_json.find("error");
    std::string err_msg = *it;
    throw Error(err_msg);
  }
}
