#pragma once

#include <boost/asio.hpp>

#include "net/app.h"

class Server
{
public:
  Server(boost::asio::io_service& service, Application& app, uint16_t port);

  //! Run the server and block forever
  void start();

private:
  //! Add an asynchronous call to accept a new connection. Once a connection is
  //! accepted, another accept_connection will be invoked automatically.
  void accept_connection();

private:
  Application& app;

  boost::asio::io_service&       service;
  boost::asio::ip::tcp::acceptor acceptor;
  boost::asio::ip::tcp::socket   socket;
};
