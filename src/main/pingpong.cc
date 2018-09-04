#include "inc/essential.h"
#include "net/server.h"

class PingPongApplication : public NetApplication
{
private:
  bool process(Buffer& read_buffer, Buffer& write_buffer) final
  {
    write_buffer << read_buffer.as_span();
    read_buffer.clear();
    return false;
  }
};

int main()
{
  PingPongApplication app;
  boost::asio::io_service service;

  Server server(service, app, 46658);
  server.start();

  service.run();
  return 0;
}
