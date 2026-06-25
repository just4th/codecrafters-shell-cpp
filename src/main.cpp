#include <iostream>
#include <string>
#include <print>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;


  


  std::string command;
  for(;;) {
    std::cout << "$ ";
    std::cin >> command;
    if (command == "exit") {
      break;
    }
    std::println("{}: command not found", command);
  }

  return 0;
}
