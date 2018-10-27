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

#include <sodium/crypto_hash_sha256.h>

#include "inc/essential.h"
#include "util/buffer.h"
#include "util/bytes.h"

template <typename T>
inline Hash sha256(gsl::span<T> data)
{
  Hash hash;
  crypto_hash_sha256((unsigned char*)hash.data(), (unsigned char*)data.data(),
                     data.size_bytes());
  return hash;
}

template <int SIZE>
inline Hash sha256(const Bytes<SIZE>& data)
{
  return sha256(data.as_span());
}

template <typename... Args>
inline Hash sha256(Args... args)
{
  Buffer buf;
  add_buffer(buf, args...);
  return sha256(buf.as_span());
}
