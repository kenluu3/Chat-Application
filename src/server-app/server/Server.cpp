#include "Server.h"

Server::Server(const std::string& port, int connections) 
  : m_dsize{ 255 }, m_port{ port }, m_connections{ connections }
{
  addrinfo hints{}, *server{ nullptr }, *p{ nullptr };

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  int gai_status{ getaddrinfo(nullptr, m_port.c_str(), &hints, &server) };

  if (gai_status != 0)
    throw std::runtime_error(gai_strerror(gai_status));
  
  for (p = server; p != nullptr; p = p->ai_next) {
    m_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

    if (m_socket == -1) {
      perror("socket");
      continue;
    }

    int yes{ 1 }; 
    if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
      perror("setsockopt");
      continue;
    }

    if (bind(m_socket, p->ai_addr, p->ai_addrlen) == -1) {
      perror("bind");
      close(m_socket);
      continue;
    }

    char ip[INET_ADDRSTRLEN]{};
    inet_ntop(p->ai_family, p->ai_addr, ip, p->ai_addrlen);
    std::cout << "|------------------ SOCKET CREATED ON IP [" << ip << "] PORT [" << m_port << "] ------------------|\n";
    break;
  }

  if (p == nullptr) 
    throw std::runtime_error("Failed to create and bind socket.");
  
  freeaddrinfo(server);

  if (listen(m_socket, m_connections) == -1) {
    perror("listen");
    throw std::runtime_error("Failed to listen.");
  }
}

Server::~Server() {
  close(m_socket);
}

void Server::start() {
  std::cout << "Waiting for a client connection...\n";    

  sockaddr_in c_addr{};
  socklen_t c_addrlen{ sizeof(c_addr) };
  int c_sock{ accept(m_socket, reinterpret_cast<sockaddr *>(&c_addr), &c_addrlen) };

  while (c_sock == -1) {
    perror("accept");
    c_sock = accept(m_socket, reinterpret_cast<sockaddr *>(&c_addr), &c_addrlen);
  }

  char c_ip[INET_ADDRSTRLEN]{};
  char r_buffer[m_dsize];
  std::memset(r_buffer, 0, m_dsize);

  inet_ntop(AF_INET, &(c_addr.sin_addr), c_ip, INET_ADDRSTRLEN);
  std::cout << "|------------------ CLIENT [" << c_ip << "] CONNECTED ------------------|\n";

  while (true) {
    long r_bytes{ recv(c_sock, r_buffer, m_dsize, 0) };
    
    if (r_bytes == 0) {
      std::cout << "|------------------ CLIENT [" << c_ip << "] TERMINATED CONNECTION ------------------|\n";
      break;
    } else {
      std::cout << "CLIENT: " << r_buffer << '\n';
      std::memset(r_buffer, 0, m_dsize);
    }

    std::string s_buffer{};
    std::getline(std::cin, s_buffer);

    if (s_buffer == "#") {
      break;
    } else {
      long s_bytes{ send(c_sock, s_buffer.c_str(), s_buffer.size(), 0) };
    }
  }
}