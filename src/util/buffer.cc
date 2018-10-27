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

#include "buffer.h"

Buffer::Buffer()
    : buf()
{
  /// Reserve 512 bytes to the buffer to prevent unnecessarily resize.
  buf.reserve(512);
}

Buffer& operator<<(Buffer& buf, uint8_t val)
{
  varint_encode(buf, val);
  return buf;
}

Buffer& operator>>(Buffer& buf, uint8_t& val)
{
  varint_decode(buf, val);
  return buf;
}

Buffer& operator<<(Buffer& buf, uint16_t val)
{
  varint_encode(buf, val);
  return buf;
}

Buffer& operator>>(Buffer& buf, uint16_t& val)
{
  varint_decode(buf, val);
  return buf;
}

Buffer& operator<<(Buffer& buf, uint32_t val)
{
  varint_encode(buf, val);
  return buf;
}

Buffer& operator>>(Buffer& buf, uint32_t& val)
{
  varint_decode(buf, val);
  return buf;
}

Buffer& operator<<(Buffer& buf, uint64_t val)
{
  varint_encode(buf, val);
  return buf;
}

Buffer& operator>>(Buffer& buf, uint64_t& val)
{
  varint_decode(buf, val);
  return buf;
}

Buffer& operator<<(Buffer& buf, const uint256_t& val)
{
  varint_encode(buf, val);
  return buf;
}

Buffer& operator>>(Buffer& buf, uint256_t& val)
{
  varint_decode(buf, val);
  return buf;
}

Buffer& operator<<(Buffer& buf, const bool val)
{
  uint8_t v = val ? 1 : 0;
  return buf << v;
}

Buffer& operator>>(Buffer& buf, bool& val)
{
  uint8_t tmp_val;
  buf >> tmp_val;

  if (tmp_val > 1)
    throw Error("Boolean variable value must be 0 or 1");

  val = (bool)tmp_val;
  return buf;
}

Buffer& operator<<(Buffer& buf, const std::string& val)
{
  return buf << (uint64_t)val.length() << gsl::make_span(val);
}

Buffer& operator>>(Buffer& buf, std::string& val)
{
  val.resize(buf.read<uint64_t>());
  return buf >> gsl::make_span(val);
}
