#include <iostream>
#include <memory>
#include <string>
#include <cstdio>
#include <cerrno>
#include <vector>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

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

  fcntl(listen_socket, F_SETFL, O_NONBLOCK);
  
  if (listen(listen_socket, MAX_CONNECTIONS) == -1) {
    perror("listen");
    return -3;
  }

  std::cout << "Waiting for client connections...\n\n";

  int communication_fd{};
  int fd_count{ 1 };
  sockaddr_in remote_addr{};
  socklen_t remote_addrlen{ sizeof(remote_addr) };
  char remoteIP[INET_ADDRSTRLEN]{};
  char buffer[256];

  std::vector<pollfd> pfds(5);

  pfds[0].fd = listen_socket;
  pfds[0].events = POLLIN;

  while (true) {
    int poll_count{ poll(&pfds[0], fd_count, -1) };
  
    if (poll_count == -1) {
      perror("poll");
      return -4;
    }

    for (int i{0}; i < fd_count; ++i) {
      if (pfds[i].revents & POLLIN) {
        if (pfds[i].fd == listen_socket) {
          communication_fd = accept(listen_socket, reinterpret_cast<sockaddr *>(&remote_addr), &remote_addrlen);

          if (communication_fd == -1) {
            perror("accept");
          } else {
            if (fd_count < 5) {
              pfds[fd_count].fd = communication_fd;
              pfds[fd_count].revents = POLLIN;

              fd_count += 1;

              inet_ntop(AF_INET, &(remote_addr.sin_addr), remoteIP, INET_ADDRSTRLEN);
              std::cout << "----------- REMOTE [" << remoteIP << "] has connected. -----------\n";
            }
          }
        }
      } else {
        long num_bytes{ recv(pfds[i].fd, buffer, sizeof buffer, 0) };
        int senderFD{ pfds[i].fd }; 

        if (num_bytes <= 0) {
          if (num_bytes == 0) {
            std::cout << "Socket " << senderFD << " has terminated connection\n";
          } else {
            perror("recv");
          }

          close(pfds[i].fd); 
          fd_count -= 1;
        } else {
          for (int j = 0; j < fd_count; ++j) {
            int dest_fd = pfds[j].fd;

            if (dest_fd != listen_socket && dest_fd != communication_fd) {
              if (send(dest_fd, buffer, num_bytes, 0) == -1) {
                perror("send");
              }
            }
          }
        }
      }
    }

  }

  close(listen_socket);

  return 0;
}