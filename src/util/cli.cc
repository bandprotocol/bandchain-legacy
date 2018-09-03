#include "cli.h"

#include <iostream>
#include <set>

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
