#include "Server.h"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "usage: chat-server <port>\n";
    return 1;
  }

  try {
    Server s{ argv[1] };

  } catch(std::runtime_error e) {
    std::cerr << e.what() << '\n';
    return 2;
  }

  return 0;
}