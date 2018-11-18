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

/// TypeID allows reversed type lookups at compile time, allowing abi interface
/// to be programmatically generated. For instance, TypeID<bool>::name yields
/// C++ string "bool".
template <typename T>
struct TypeID {
  static const char* name;
};

#define TYPEID(type)                                                           \
  template <>                                                                  \
  inline const char* TypeID<type>::name = #type;

#define TYPEID_CUSTOM(type, custom_name)                                       \
  template <>                                                                  \
  inline const char* TypeID<type>::name = #custom_name;

TYPEID(void)
TYPEID(bool)
TYPEID(int)
TYPEID(uint8_t)
TYPEID(uint16_t)
TYPEID(uint64_t)
TYPEID(uint256_t)
TYPEID_CUSTOM(std::string, String)

class Buffer;
TYPEID(Buffer)

class Equation;
TYPEID(Equation)

template <int SIZE>
class Bytes;
TYPEID_CUSTOM(Bytes<20>, Address)
TYPEID_CUSTOM(Bytes<32>, Hash)
TYPEID_CUSTOM(Bytes<64>, Signature)

#undef TYPEID_CUSTOM
#undef TYPEID
