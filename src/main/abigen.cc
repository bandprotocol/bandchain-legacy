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

#include <iostream>

#include "inc/essential.h"
#include "util/json.h"
#include "util/msg.h"

int main()
{
  json interface;

#define ADD_TOP_LEVEL_INTERFACE(MSG)                                           \
  interface[BOOST_PP_STRINGIZE(MSG)] = MSG::interface();

  ADD_TOP_LEVEL_INTERFACE(GenesisMsg)
  ADD_TOP_LEVEL_INTERFACE(BlockMsg)
  ADD_TOP_LEVEL_INTERFACE(HeaderMsg)
#undef ADD_TOP_LEVEL_INTERFACE

#define ADD_TX_INTERFACE(R, _, TX)                                             \
  {                                                                            \
    json txInterface;                                                          \
    txInterface["Input"] = BAND_MACRO_MSG(TX)::interface();                    \
    txInterface["Output"] = BAND_MACRO_RESPONSE(TX)::interface();              \
    interface[BOOST_PP_STRINGIZE(TX)] = txInterface;                           \
  }

  BAND_MACRO_MESSAGE_FOR_EACH(ADD_TX_INTERFACE)
#undef ADD_TX_INTERFACE

  std::cout << interface.dump(2) << std::endl;
  return 0;
}
