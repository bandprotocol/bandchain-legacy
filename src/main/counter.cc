#include "net/server.h"
#include "tendermint/tmapp.h"

int main()
{
  TendermintApplication app;
  boost::asio::io_service service;

  Server server(service, app, 46658);
  server.start();

  service.run();
  return 0;
}
