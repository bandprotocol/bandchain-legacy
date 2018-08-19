#include "band/app.h"
#include "net/server.h"

int main()
{
  BandApplication app;
  boost::asio::io_service service;

  Server server(service, app, 46658);
  server.start();

  service.run();
  return 0;
}
