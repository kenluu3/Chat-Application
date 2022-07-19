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
    std::getline(std::cin, send_msg);

    if (send_msg == "Q") {
      break;
    } else {
      long bytesSent{ send(comm_socket, send_msg.c_str(), send_msg.size(), 0) };
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