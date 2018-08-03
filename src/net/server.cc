#include "server.h"

#include "net/session.h"

using boost::asio::ip::tcp;

Server::Server(boost::asio::io_service& _service, NetApplication& _app,
               uint16_t port)
    : app(_app)
    , service(_service)
    , acceptor(service, tcp::endpoint(tcp::v4(), port))
    , socket(service)
{
}

void Server::start() { accept_connection(); }

void Server::accept_connection()
{
  acceptor.async_accept(socket, [this](const auto error_code) {
    if (!error_code) {
      std::make_shared<Session>(std::move(socket), app)->serve();
      accept_connection();
    } else {
      throw std::runtime_error("Failed to accept socket connection");
    }
  });
}
