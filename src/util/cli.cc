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

#include "cli.h"

#include <iostream>

Cmd::Cmd(const char* desc, int argc, char* argv[])
    : options(argv[0], desc)
{
  for (const CmdArgBase* arg : args) {
    arg->addOption(options);
  }

  options.add_options()("h,help", "run in verbose mode");
  options.add_options()("v,verbose", "show verbose logging messages");

  parseResult.emplace(options.parse(argc, argv));

  if (parseResult->count("h")) {
    std::cout << options.help() << std::endl;
    exit(0);
  }

  if (parseResult->count("v")) {
    spdlog::set_level(spdlog::level::debug);
  }
}

CmdArgBase::CmdArgBase(const std::string& _opts,
                       const std::string& _desc,
                       const std::string& _defaultStr)
    : opts(_opts)
    , desc(_desc)
    , defaultStr(_defaultStr)
    , key(_opts.size() >= 2 && _opts[1] == ',' ? _opts.substr(0, 1) : _opts)
{
  if (opts.find(' ') != std::string::npos) {
    throw Failure("CmdArg opts \"{}\" must not contain space", opts);
  }

  Cmd::args.push_back(this);
}

bool CmdArgBase::given() const
{
  return Cmd::parseResult->count(key) > 0;
}
