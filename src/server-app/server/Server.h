#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <cstdio>
#include <cerrno>
#include <stdexcept>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

class Server {
  private:
    std::string m_port; 
    int m_connections, m_dsize, m_socket;

  public:
    Server(const std::string& port = "5000", int connections = 1);
    ~Server();

    void start();
};