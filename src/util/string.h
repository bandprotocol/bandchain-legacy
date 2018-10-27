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

template <typename T>
inline std::string bytes_to_hex(gsl::span<T> data)
{
  static_assert(sizeof(T) == 1);
  std::string hex;
  for (auto b : data) {
    hex += "{:02x}"_format(static_cast<unsigned char>(b));
  }
  return hex;
}

inline std::string string_to_hex(const std::string& data)
{
  return bytes_to_hex(gsl::make_span(data));
}
