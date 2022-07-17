#include <iostream>
#include <string>
#include <cstdio>
#include <cerrno>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define PORT "3490"
#define MAX_DATA_SIZE 100

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: client <ip-address>\n";
    return -1;
  }

  addrinfo hints{}, *server{};
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  int result{ getaddrinfo(argv[1], PORT, &hints, &server) };

  if (result != 0) {
    std::cerr << "Error retrieving server information: " << gai_strerror(result) << '\n';
    return -1;
  }

  int comm{};
  addrinfo *p{ nullptr };

  for (p = server; p != nullptr; p = p->ai_next) {
    if ((comm = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("socket");
      continue;
    }

    if (connect(comm, p->ai_addr, p->ai_addrlen) == -1) {
      close(comm);
      perror("connect");
      continue;
    }

    char ipAddress[INET_ADDRSTRLEN];
    inet_ntop(p->ai_family, p->ai_addr, ipAddress, p->ai_addrlen);

    std::cout << "Created Socket to and successfully connected to IP Address: " << ipAddress << " on Port: " << PORT << '\n';
    break;
  }

  freeaddrinfo(server);

  if (p == nullptr) {
    std::cout << "Failed to connect to server.\n";
    return -2;
  }

  char buffer[MAX_DATA_SIZE];
  long numBytes{ recv(comm, buffer, MAX_DATA_SIZE - 1, 0) };

  if (numBytes == -1) {
    perror("recv");
    return -3;
  }

  std::cout << "Server sent: " << buffer << '\n';

  close(comm);

  return 0;
}