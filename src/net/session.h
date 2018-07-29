#pragma once

#include <boost/asio.hpp>

#include "net/app.h"
#include "util/buffer.h"

class Session : public std::enable_shared_from_this<Session>
{
public:
  Session(boost::asio::ip::tcp::socket socket, Application& app);

  void serve();

private:
  void process_read(std::size_t length);
  void process_write(std::size_t length);

  void async_read();
  void async_write();

private:
  Application&                 app;
  boost::asio::ip::tcp::socket socket;

  Buffer read_buffer;
  Buffer write_buffer;

  std::byte socket_buffer[5];
};
