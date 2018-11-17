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

#include "util/bytes.h"
#include "util/msg.h"

/// Base Band Protocol listener. Any listener on Band Transaction must be a
/// subclass of this datatype.
class BaseListener
{
public:
  virtual ~BaseListener()
  {
  }

  /// Load the current state of this listener. May involve blocking remote
  /// database calls.
  virtual void load()
  {
  }

  /// Begin a new block. The listener may override this function to perform
  /// necessary transactional operations.
  virtual void begin(uint64_t timestamp, const Address& proposer)
  {
  }

  /// Commit the current block.
  virtual void commit()
  {
  }

  /// "handle<MsgType>" functions are implemented as no-op functions. These
  /// functions can be overriden by the subclass to give functionalities.
#define BASE_PROCESS_MESSAGE(R, _, MSG)                                        \
  virtual void BAND_MACRO_HANDLE(MSG)(const BAND_MACRO_MSG(MSG) & msg)         \
  {                                                                            \
    /* Nothing interesting here! */                                            \
  }

  BAND_MACRO_MESSAGE_FOR_EACH(BASE_PROCESS_MESSAGE)

#undef BASE_PROCESS_MESSAGE
};
