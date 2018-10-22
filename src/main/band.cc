#include "band/app.h"
#include "inc/essential.h"
#include "net/server.h"
#include "store/context.h"
#include "store/storage_map.h"
#include "store/storage_rocksdb.h"
#include "util/cli.h"

CmdArg<bool> use_db("use-db", "set this flag to use rocksdb");

int main(int argc, char* argv[])
{
  Cmd cmd(argc, argv);
  LOG("BAND blockchain appliation starting...");

  std::unique_ptr<Storage> store;
  if (+use_db) {
    store = std::make_unique<StorageDB>();
  } else {
    store = std::make_unique<StorageMap>();
  }
  Context ctx(*store);
  BandApplication app(ctx);
  boost::asio::io_service service;

  Server server(service, app, 26658);
  server.start();

  service.run();
  return 0;
}
