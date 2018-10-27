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
#include <set>

CmdArg<bool> verbose_mode("verbose", "run in verbose mode", 'v');

Cmd::Cmd(int argc, char* argv[])
{
  CmdArgBase* cmd_arg = nullptr;

  for (int idx = 1; idx < argc; ++idx) {
    std::string arg = argv[idx];

    if (arg == "--help") {
      std::cout << help();
      exit(0);
    }

    if (auto it = cmd_map.find(arg); it != cmd_map.end()) {
      if (cmd_arg != nullptr) {
        cmd_arg->parse("");
        cmd_arg->passed = true;
        cmd_arg = nullptr;
      }
      cmd_arg = it->second;
    } else {
      if (cmd_arg != nullptr) {
        cmd_arg->parse(arg);
        cmd_arg->passed = true;
        cmd_arg = nullptr;
      } else {
        args.push_back(arg);
      }
    }
  }

  if (cmd_arg != nullptr) {
    cmd_arg->parse("");
    cmd_arg->passed = true;
    cmd_arg = nullptr;
  }

  if (+verbose_mode)
    spdlog::set_level(spdlog::level::debug);
}

std::string Cmd::help() const
{
  std::string ret;
  std::set<CmdArgBase*> printed;

  for (auto& [key, cmd] : cmd_map) {
    (void)key;

    if (printed.insert(cmd).second) {
      ret += cmd->help();
    }
  }

  return ret;
}

CmdArgBase::CmdArgBase(std::string _name, std::string _desc, char _abbrev)
    : name(std::move(_name))
    , desc(std::move(_desc))
    , abbrev(_abbrev)
{
  add_to_cmd("--{}"_format(name));
  if (_abbrev != '\0')
    add_to_cmd("-{}"_format(abbrev));
}

std::string CmdArgBase::help() const
{
  std::string abbrev_str;
  if (abbrev != '\0')
    abbrev_str = " [-{}]"_format(abbrev);
  return "  {}{} : {} ({})\n"_format(name, abbrev_str, desc, get_type());
}

void CmdArgBase::add_to_cmd(const std::string& key)
{
  if (!Cmd::cmd_map.try_emplace(key, this).second)
    throw Failure("Duplicate cmd arg key: {}", key);
}
