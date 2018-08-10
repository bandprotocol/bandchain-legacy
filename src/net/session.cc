#include "session.h"

Session::Session(boost::asio::ip::tcp::socket _socket, NetApplication& _app)
    : app(_app)
    , socket(std::move(_socket))
{
}

void Session::serve() { async_read(); }

void Session::process_read(size_t length)
{
  read_buffer.append(socket_buffer, length);
  while (app.process(read_buffer, write_buffer))
    ;
  async_write();
}

void Session::process_write(size_t length)
{
  write_buffer.consume(length);
  async_read();
}

void Session::async_read()
{
  boost::asio::async_read(
      socket, boost::asio::buffer(socket_buffer, sizeof(socket_buffer)),
      boost::asio::transfer_at_least(1),
      [self = shared_from_this()](const auto ec, size_t length) {
        if (!ec) {
          self->process_read(length);
        } else {
          log::info("hey {}", ec.message());
          throw std::runtime_error("Failed to read from socket");
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
      [self = shared_from_this()](const auto ec, size_t length) {
        if (!ec) {
          self->process_write(length);
        } else {
          throw std::runtime_error("Failed to write to socket");
        }
      });
}
