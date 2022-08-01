#include "Client.h"

Client::Client(const std::string& ip, const std::string& port) 
  : m_dsize{ 255 }
{
  addrinfo hints{}, *server{ nullptr }, *p{ nullptr };

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  int gai_status{ getaddrinfo(ip.c_str(), port.c_str(), &hints, &server) }; 

  if (gai_status != 0) 
    throw std::runtime_error(gai_strerror(gai_status));

  for (p = server; p != nullptr; p = p->ai_next) {
    m_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

    if (m_socket == -1) {
      perror("socket");
      continue;
    }

    if (connect(m_socket, p->ai_addr, p->ai_addrlen) == -1) {
      close(m_socket);
      perror("connect");
      continue;
    }

    break;
  }
  
  if (p == nullptr) 
    throw std::runtime_error("Failed to connect to server.");
  
  freeaddrinfo(server);

  std::cout << "|------------------ CONNECTED TO [" << ip << "] ------------------|\n";
}

Client::~Client() {
  close(m_socket);
};

void Client::start() {
  char r_buffer[m_dsize];
  std::memset(r_buffer, 0, m_dsize);

  while (true) {
    std::string s_buffer{};
    std::getline(std::cin, s_buffer);

    if (s_buffer == "#") {
      break;
    } else {
      long s_bytes{ send(m_socket, s_buffer.c_str(), s_buffer.size(), 0) };
    }

    long r_bytes{ recv(m_socket, r_buffer, m_dsize, 0) };
    if (r_bytes == 0) {
      std::cout << "|------------------ SERVER TERMINATED CONNECTION ------------------|\n";
      break;
    } else {
      std::cout << "SERVER: " << r_buffer << '\n';
      std::memset(r_buffer, 0, m_dsize);
    }
  }
}