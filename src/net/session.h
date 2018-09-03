#pragma once

#include <boost/asio.hpp>

#include "inc/essential.h"
#include "net/netapp.h"
#include "util/buffer.h"

class Session : public std::enable_shared_from_this<Session>
{
public:
  Session(boost::asio::ip::tcp::socket socket, NetApplication& app);

  void serve();

private:
  void process_read(size_t length);
  void process_write(size_t length);

  void async_read();
  void async_write();

private:
  NetApplication& app;
  boost::asio::ip::tcp::socket socket;

  Buffer read_buffer;
  Buffer write_buffer;

  std::byte socket_buffer[1024];
};
