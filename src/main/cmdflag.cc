#include "util/cli.h"

CmdArg<bool> arg_bool("arg_b", "test bool");
CmdArg<std::string> arg_str("arg_s", "test string", 's');

int main(int argc, char* argv[])
{
  Cmd cmd(argc, argv);
  log::info("{} {}", arg_bool.given(), +arg_bool);
  log::info("{} {}", arg_str.given(), +arg_str);
  return 0;
}
