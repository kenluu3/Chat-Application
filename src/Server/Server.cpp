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
#define MAX_CONNECTIONS 15

int main(int argc, char* argv[]) {
  addrinfo hints{}, *server{ nullptr };
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  int result{ getaddrinfo(NULL, PORT, &hints, &server) };

  if (result != 0) {
    std::cerr << "Error retrieving server information: " << gai_strerror(result) << '\n';
    return -1;
  }

  int listener{}; 
  addrinfo *p{ nullptr };

  for (p = server; p != nullptr; p = p->ai_next) {
    if ((listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("socket");
      continue;
    }

    int y{1};
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(y)) == -1) {
      perror("setsockopt");
      continue;
    }

    if (bind(listener, p->ai_addr, p->ai_addrlen) == -1) {
      close(listener);
      perror("bind");
      continue;
    }

    char ipAddress[INET_ADDRSTRLEN];
    inet_ntop(p->ai_family, p->ai_addr, ipAddress, p->ai_addrlen);

    std::cout << "Created Socket and Binded to IP Address: " << ipAddress << " on Port: " << PORT << '\n'; 
    break;
  }

  if (p == nullptr) {
    std::cout << "Failed to create and bind socket.\n";
    return -2;
  }

  freeaddrinfo(server);

  if (listen(listener, MAX_CONNECTIONS) == -1) {
    perror("listen");
    return -3;
  }

  std::cout << "Listening for connections...\n";

  sockaddr_in client{};
  socklen_t clientsize{ sizeof(client) };
  int comm{ accept(listener, reinterpret_cast<sockaddr *>(&client), &clientsize) };

  if (comm == -1) {
    perror("accept");
    return -4;
  }

  close(listener);

  char clientAddress[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(client.sin_addr), clientAddress, INET_ADDRSTRLEN);
  std::cout << "Client connected: " << clientAddress << '\n';

  send(comm, "Hello There!", 13, 0);

  close(comm);

  return 0;
}