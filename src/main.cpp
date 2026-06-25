#include <iostream>
#include <sstream>
#include <string>
#include <print>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::string command;
  std::string arg;

  for(;;) {
    std::cout << "$ ";

    std::getline(std::cin, command);
    std::stringstream stream(command);

    if (!(stream >> arg)) {
      continue;
    }

    if (arg == "exit") {
      break;
    }
    if (arg == "echo") {
      if (stream >> arg) {
        std::print("{}", arg);
          while(stream >> arg) {
            std::print(" {}", arg);
          }
      }
      std::println();
      continue;
    }

    std::println("{}: command not found", command);
  }

  return 0;
}
