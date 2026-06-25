#include <iostream>
#include <sstream>
#include <string>
#include <print>
#include <map>
#include <functional>

using CommandHandler = std::function<void(std::stringstream&&)>;

void EchoHandler(std::stringstream&& stream);
void TypeHandler(std::stringstream&& stream);

const std::unordered_map<std::string, const CommandHandler> HANDLERS = {
  {"echo", EchoHandler},
  {"type", TypeHandler},
  {"exit", {}}
};

void EchoHandler(std::stringstream&& stream) {
  std::string arg;
  if (!(stream >> arg)) {
    return;
  }

  std::print("{}", arg);
  while(stream >> arg) {
    std::print(" {}", arg);
  }
}

void TypeHandler(std::stringstream&& stream) {
  std::string arg;
  if (stream >> arg) {
    if (HANDLERS.contains(arg)) {
      std::print("{} is a shell builtin", arg);
    } else {
      std::print("{}: not found", arg);
    }
  }
  return;
}

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  for(;;) {
    std::print("$ ");

    std::string command;
    std::getline(std::cin, command);
    std::stringstream stream(command);

    std::string name;
    if (!(stream >> name)) {
      continue;
    }

    if (name == "exit") {
      break;
    }

    auto it = HANDLERS.find(name);
    if (it == HANDLERS.end()) {
      std::println("{}: command not found", name);
      continue;
    }

    it->second(std::move(stream));
    std::println();
  }

  return 0;
}
