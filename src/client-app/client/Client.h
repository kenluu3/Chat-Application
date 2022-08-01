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

class Client {
  private:
    int m_dsize, m_socket;

  public:
    Client(const std::string& ip = "0.0.0.0", const std::string& port = "5000");
    ~Client();

    void start();
};