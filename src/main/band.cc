#include "band/app.h"
#include "inc/essential.h"
#include "net/server.h"
#include "store/context_db.h"
#include "store/context_map.h"
#include "util/cli.h"

CmdArg<bool> use_db("use-db", "set this flag to use rocksdb");

int main(int argc, char* argv[])
{
  Cmd cmd(argc, argv);

  std::unique_ptr<Context> ctx;
  if (+use_db) {
    log::info("Use rocksDB");
    ctx = std::make_unique<ContextDB>();
  } else {
    log::info("Use map");
    ctx = std::make_unique<ContextMap>();
  }

  // ContextDB ctx("/tmp/tempdb");
  BandApplication app(*ctx);
  boost::asio::io_service service;

  Server server(service, app, 26658);
  server.start();

  service.run();
  return 0;
}
