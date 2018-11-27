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

#include <array>
#include <boost/functional/hash.hpp>

#include "inc/essential.h"
#include "util/buffer.h"

/// Bytes is a stack-only data structure that encapsulates an array of raw
/// bytes. The structure is templated over the size of the container and
/// provides useful methods. Bytes should be passed by reference for efficiency.
template <int SIZE>
class Bytes
{
public:
  static constexpr size_t Size = SIZE;

  Bytes() = default;
  Bytes(const Bytes& bytes) = default;

  /// Create an n-byte structure from the given span. Throw on invalid size.
  Bytes(gsl::span<const byte> data);

  /// Create an n-byte structure from a raw string
  static Bytes raw(const std::string& raw_string);

  /// Parse a hex string into an n-byte structure
  static Bytes hex(const std::string& hex_string);

  /// Create an arbitrary n-byte structure. Useful for testing
  static Bytes rand();

  /// Simple comparison operators
  bool operator==(const Bytes& rhs) const;
  bool operator!=(const Bytes& rhs) const;

  /// Convenient function to check if all bits are zeroes
  bool empty() const;

  /// Return a friendly hex representation of this bytes value
  std::string to_string() const;

  /// Expose this bytes structure as a mutating span
  gsl::span<byte> as_span();

  /// Expose this bytes structure as a non-mutating span
  gsl::span<const byte> as_const_span() const;

  friend Buffer& operator<<(Buffer& buf, const Bytes& data)
  {
    return buf << data.as_const_span();
  }

  friend Buffer& operator>>(Buffer& buf, Bytes& data)
  {
    return buf >> data.as_span();
  }

private:
  std::array<byte, SIZE> rawdata_{};

} __attribute__((packed));

using Address = Bytes<20>;   //< 20-Byte address
using Hash = Bytes<32>;      //< SHA-256 hash value
using VerifyKey = Bytes<32>; //< Ed25519 verify key
using SecretKey = Bytes<64>; //< Ed25519 secret key
using Signature = Bytes<64>; //< Ed25519 signature

namespace std
{
template <int SIZE>
struct hash<Bytes<SIZE>> {
  inline size_t operator()(const Bytes<SIZE>& obj) const
  {
    gsl::span<const byte> obj_span = obj.as_const_span();
    return boost::hash_range(obj_span.data(), obj_span.data() + SIZE);
  }
};
} // namespace std
