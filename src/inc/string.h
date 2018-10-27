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

#include <spdlog/fmt/bundled/ostream.h>

using namespace fmt::literals;

template <typename...>
using void_t = void;

template <typename T, typename = void>
struct Has_to_string : std::false_type {
};

template <typename T>
struct Has_to_string<
    T, void_t<std::enable_if_t<std::is_same<
           std::string, decltype(std::declval<T>().to_string())>::value>>>
    : std::true_type {
};

template <typename T, typename = std::enable_if_t<Has_to_string<T>::value>>
std::ostream& operator<<(std::ostream& os, const T& obj)
{
  os << obj.to_string();
  return os;
}
