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

#include "inc/essential.h"
#include "util/buffer.h"
#include "util/typeid.h"

// enum telling string is case sensitive or not.
enum class StringCase : uint8_t {
  Sensitive,
  InSensitive,
};

// Class String is a custom string type containing maximum length. Every
// character is readable. It changes string to lower case if it's not case
// sensitive.
template <int MAX_LENGTH, StringCase CASE>
class String
{
public:
  static constexpr size_t MaxLegth = MAX_LENGTH;

  String() = default;
  String(const String& string) = default;
  String(String&& string) = default;

  String(const std::string& _string);

  String& operator=(const String& _string) = default;
  String& operator=(String&& _string) = default;

  bool empty() const
  {
    return rawdata.empty();
  }

  std::string to_string() const;

  friend Buffer& operator<<(Buffer& buf, const String& data)
  {
    return buf << data.rawdata;
  }

  friend Buffer& operator>>(Buffer& buf, String& data)
  {
    buf >> data.rawdata;
    data.validate();
    return buf;
  }

private:
  // Validate rawdata that size doesn't exceed max_length and every character
  // is readable.
  void validate();

private:
  std::string rawdata{};
};

using Ident = String<20, StringCase::InSensitive>; //< 20-char readable ident

TYPEID(Ident)

using NodeID =
    String<128, StringCase::Sensitive>; //< 128-length string represent node in
                                        // graph database
TYPEID(NodeID)
