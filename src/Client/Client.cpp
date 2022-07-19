#include <iostream>
#include <memory>
#include <string>
#include <cstdio>
#include <cerrno>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr << "USAGE: `chatroom-client <ip-address> <port>\n";
    return -1;
  }

  constexpr int MAX_DATA{ 100 };
  addrinfo hints{}, *server{ nullptr }, *itr{ nullptr };
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  int gai{ getaddrinfo(argv[1], argv[2], &hints, &server) }; 
  if (gai != 0) {
    std::cerr << "getaddrinfo: " << gai_strerror(gai) << '\n';
    return -1;
  }

  int comm_socket{};
  for (itr = server; itr != nullptr; itr = itr->ai_next) {
    comm_socket = socket(itr->ai_family, itr->ai_socktype, itr->ai_protocol);
    
    if (comm_socket == -1) {
      perror("socket");
      continue;
    } 

    if (connect(comm_socket, itr->ai_addr, itr->ai_addrlen) == -1) {
      close(comm_socket);
      perror("connect");
      continue;
    }

    break;
  }

  if (itr == nullptr) {
    std::cerr << "Failed to connect to server: " << argv[1] << '\n';
    return -2;
  }

  std::cout << "----------- CONNECTED TO [" << argv[1] << "] -----------\n";

  freeaddrinfo(server);

  char read_buffer[MAX_DATA]{};
  long read_bytes{1};

  while (read_bytes) {
    std::string send_msg{};
    std::cin >> send_msg;

    if (send_msg == "Q") {
      break;
    } else {
      send(comm_socket, send_msg.c_str(), send_msg.size(), 0);
    }

    std::memset(read_buffer, 0, MAX_DATA);
    read_bytes = recv(comm_socket, read_buffer, MAX_DATA, 0);

    if (read_bytes == 0) {
      std::cout << "SERVER [" << argv[1] << "] has terminated connection.\n";
      break;
    } else {
      std::cout << "SERVER: " << read_buffer << '\n';
    }
  }

  close(comm_socket);

  return 0;
}


/* 
#include <iostream>
#include <string>
#include <cstdio>
#include <cerrno>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define PORT "3500"
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

    break;
  }

  freeaddrinfo(server);

  if (p == nullptr) {
    std::cout << "Failed to connect to server.\n";
    return -2;
  }

  char buffer[MAX_DATA_SIZE];
  long numBytes{ recv(comm, buffer, MAX_DATA_SIZE, 0) };

  if (numBytes == -1) {
    perror("recv");
    return -3;
  }

  std::cout << buffer << '\n';

  while (numBytes > 0) {
    std::string msg{};
    std::cin >> msg;

    if (msg == "q") {
      break;
    } else {
      send(comm, msg.c_str(), msg.size(), 0);
    }

    memset(buffer, 0, MAX_DATA_SIZE);
    numBytes = recv(comm, buffer, MAX_DATA_SIZE, 0);

    if (numBytes == 0) {
      std::cout << "Connection Closed\n";
    } else {
      std::cout << "Server: " << buffer << '\n';
    }

  }

  close(comm);

  return 0;
} 
*/