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

#include "inc/essential.h"
#include "util/cli.h"

CmdArg<bool> arg_bool("arg_b", "test bool");
CmdArg<std::string> arg_str("s,arg_s", "test string");

int main(int argc, char* argv[])
{
  Cmd cmd("CmdFlag test program", argc, argv);
  LOG("{} {}", arg_bool.given(), +arg_bool);
  LOG("{} {}", arg_str.given(), +arg_str);
  return 0;
}
