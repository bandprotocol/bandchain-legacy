#include "inc/essential.h"
#include "util/cli.h"

CmdArg<bool> arg_bool("arg_b", "test bool");
CmdArg<std::string> arg_str("arg_s", "test string", 's');

int main(int argc, char* argv[])
{
  Cmd cmd(argc, argv);
  LOG("{} {}", arg_bool.given(), +arg_bool);
  LOG("{} {}", arg_str.given(), +arg_str);
  return 0;
}
