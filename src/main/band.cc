#include "band/app.h"
#include "net/server.h"
#include "store/context_map.h"

int main()
{
  ContextMap ctx;
  BandApplication app(ctx);
  boost::asio::io_service service;

  Server server(service, app, 26658);
  server.start();

  service.run();
  return 0;
}
