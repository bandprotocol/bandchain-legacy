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

#include "bytes.h"

#include "crypto/random.h"

namespace
{
inline byte hex_to_byte(char hex_digit)
{
  if ('0' <= hex_digit && hex_digit <= '9') {
    return byte(hex_digit - '0');
  } else if ('a' <= hex_digit && hex_digit <= 'f') {
    return byte(hex_digit - 'a' + 10);
  } else if ('A' <= hex_digit && hex_digit <= 'F') {
    return byte(hex_digit - 'A' + 10);
  } else {
    throw Error("hex_to_bytes: Invalid hex digit character");
  }
}
} // namespace

template <int SIZE>
Bytes<SIZE>::Bytes(gsl::span<const byte> data)
{
  if (data.size() != SIZE)
    throw Error("Bytes<{}>(span): Invalid span size {}", SIZE, data.size());

  std::copy_n(data.begin(), SIZE, rawdata_.begin());
}

template <int SIZE>
Bytes<SIZE> Bytes<SIZE>::raw(const std::string& raw_string)
{
  if (raw_string.size() != SIZE) {
    throw Error("Bytes<{}>::raw: Invalid raw string length {}", SIZE,
                raw_string.size());
  }

  Bytes<SIZE> ret;
  std::memcpy(ret.rawdata_.data(), raw_string.c_str(), SIZE);
  return ret;
}

template <int SIZE>
Bytes<SIZE> Bytes<SIZE>::hex(const std::string& hex_string)
{
  if (hex_string.size() != 2 * SIZE) {
    throw Error("Bytes<{}>::hex: Invalid hex string length {}", SIZE,
                hex_string.size());
  }

  Bytes<SIZE> ret;
  for (size_t i = 0; i < SIZE; ++i) {
    ret.rawdata_[i] = (hex_to_byte(hex_string[2 * i + 0]) << 4) |
                      (hex_to_byte(hex_string[2 * i + 1]) << 0);
  }
  return ret;
}

template <int SIZE>
Bytes<SIZE> Bytes<SIZE>::rand()
{
  Bytes<SIZE> ret;
  random_bytes(ret.as_span());
  return ret;
}

template <int SIZE>
bool Bytes<SIZE>::operator==(const Bytes<SIZE>& rhs) const
{
  return rawdata_ == rhs.rawdata_;
}

template <int SIZE>
bool Bytes<SIZE>::operator!=(const Bytes<SIZE>& rhs) const
{
  return rawdata_ != rhs.rawdata_;
}

template <int SIZE>
bool Bytes<SIZE>::empty() const
{
  return operator==(Bytes());
}

template <int SIZE>
std::string Bytes<SIZE>::to_string() const
{
  return bytes_to_hex(as_const_span());
}

template <int SIZE>
gsl::span<byte> Bytes<SIZE>::as_span()
{
  return gsl::make_span(rawdata_);
}

template <int SIZE>
gsl::span<const byte> Bytes<SIZE>::as_const_span() const
{
  return gsl::make_span(rawdata_);
}

template class Bytes<20>;
template class Bytes<32>;
template class Bytes<64>;
