#include "Client.h"

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr << "usage: chat-client <ip-address> <port>\n";
    return 1;
  }

  try {
    Client c{ argv[1], argv[2] };
    c.start();
  } catch (std::runtime_error e) {
    std::cerr << e.what() << '\n';
    return 2;
  }

  return 0;
}