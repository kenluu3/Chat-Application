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
  const std::string PORT{ "3500" };
  constexpr int MAX_DATA{ 100 };
  constexpr int MAX_CONNECTIONS{ 15 };

  char ip[INET_ADDRSTRLEN]{};
  addrinfo hints{}, *server{ nullptr }, *itr{ nullptr };
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  int gai{ getaddrinfo(NULL, PORT.c_str(), &hints, &server) };
  if (gai != 0) {
    std::cerr << "getaddrinfo: " << gai_strerror(gai) << '\n';
    return -1;
  }

  int listen_socket{};
  for (itr = server; itr != nullptr; itr = itr->ai_next) {
    listen_socket = socket(itr->ai_family, itr->ai_socktype, itr->ai_protocol);

    if (listen_socket == -1) {
      perror("socket");
      continue;
    }

    int yes{1};
    if (setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
      perror("setsockopt");
      continue;
    }

    if (bind(listen_socket, itr->ai_addr, itr->ai_addrlen) == -1) {
      close(listen_socket);
      perror("bind");
      continue;
    }

    inet_ntop(itr->ai_family, itr->ai_addr, ip, itr->ai_addrlen);
    std::cout << "Successfully created server socket binded to IP Address: " << ip << " on Port: " << PORT << '\n';
    break;
  }

  freeaddrinfo(server);

  if (itr == nullptr) {
    std::cerr << "Failed to create server socket\n";
    return -2;
  }
  
  if (listen(listen_socket, MAX_CONNECTIONS) == -1) {
    perror("listen");
    return -3;
  }

  std::cout << "Waiting for client connections...\n\n";

  sockaddr_in client{};
  socklen_t client_size{ sizeof(client) };
  char client_ip[INET_ADDRSTRLEN];

  int comm_socket{ accept(listen_socket, reinterpret_cast<sockaddr *>(&client), &client_size) };
  
  close(listen_socket);

  if (comm_socket == -1) {
    perror("accept");
    return -4;
  }

  inet_ntop(AF_INET, &(client.sin_addr), client_ip, INET_ADDRSTRLEN);
  std::cout << "----------- CLIENT [" << client_ip << "] has connected. -----------\n";

  char read_buffer[MAX_DATA]{};
  long read_bytes{1};

  while (read_bytes) {
    std::memset(read_buffer, 0, MAX_DATA);
    read_bytes = recv(comm_socket, read_buffer, MAX_DATA, 0);

    if (read_bytes == 0) {
      std::cout << "CLIENT [" << client_ip << "] has terminated connection.\n";
      break;
    } else {
      std::cout << "CLIENT: " << read_buffer << std::endl;
    }

    std::string send_msg{};
    std::cin >> send_msg;

    if (send_msg == "Q") {
      break;
    } else {
      send(comm_socket, send_msg.c_str(), send_msg.size(), 0);
    }
  }

  close(comm_socket);

  return 0;
}