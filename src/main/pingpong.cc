#include "net/server.h"

class PingPongApplication : public NetApplication
{
private:
  void process(Buffer& read_buffer, Buffer& write_buffer) final
  {
    write_buffer.append(read_buffer);
    read_buffer.clear();
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
