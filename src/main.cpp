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
    std::getline(std::cin, command);
    std::println("{}: command not found", command);
  }

}
