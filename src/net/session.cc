#include "session.h"

#include <iostream>

Session::Session(boost::asio::ip::tcp::socket _socket, Application& _app)
    : app(_app)
    , socket(std::move(_socket))
{
}

void Session::serve()
{
  async_read();
}

void Session::process_read(std::size_t length)
{
  read_buffer.append(socket_buffer, length);
  app.process(read_buffer, write_buffer);
  async_write();
}

void Session::process_write(std::size_t length)
{
  write_buffer.consume(length);
  async_read();
}

void Session::async_read()
{
  socket.async_read_some(
      boost::asio::buffer(socket_buffer, sizeof(socket_buffer)),
      [self = shared_from_this()](const auto ec, std::size_t length) {
        if (!ec) {
          self->process_read(length);
        } else {
          // TODO: log_err
        }
      });
}

void Session::async_write()
{
  if (write_buffer.empty()) {
    return async_read();
  }

  boost::asio::async_write(
      socket, boost::asio::buffer(write_buffer.begin(), write_buffer.size()),
      [self = shared_from_this()](const auto ec, std::size_t length) {
        if (!ec) {
          self->process_write(length);
        } else {
          // TODO: log_err
        }
      });
}
