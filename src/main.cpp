#include <iostream>
#include <sstream>
#include <string>
#include <print>
#include <map>
#include <functional>
#include <filesystem>


consteval char PATH_separator() {
  #ifdef __WIN32
    return ';';
  #else
    return ':';
  #endif
}

using CommandHandler = std::function<void(std::istringstream&&)>;

void process_PATH(std::function<bool(std::filesystem::path&&)>);
void EchoHandler(std::istringstream&&);
void TypeHandler(std::istringstream&&);

const std::unordered_map<std::string, const CommandHandler> HANDLERS = {
  {"exit", {}},
  {"echo", EchoHandler},
  {"type", TypeHandler},
};

std::vector<std::string> read_args(std::istringstream& stream) {
  std::vector<std::string> res;
  std::string arg;
  while (stream >> arg) {
    res.push_back(std::move(arg));
  }
  return res;
}

void EchoHandler(std::istringstream&& stream) {
  const auto args = read_args(stream);

  if (args.empty()) {
    return;
  }
  std::print("{}", args[0]);
  for (auto it = args.begin() + 1; it != args.end(); ++it) {
    std::print(" {}", *it);
  }
}

void TypeHandler(std::istringstream&& stream) {
  namespace fs = std::filesystem;

  const auto args = read_args(stream);
  const auto name = args[0];

  if (HANDLERS.contains(name)) {
    std::print("{} is a shell builtin", name);
    return;
  }
  fs::path exec;
  auto check_exec = [&exec, &name](fs::path&& dir) {
    dir.append(name);
    auto st = fs::status(dir);
    if (fs::is_regular_file(st)) {
      return false;
    }

    auto exec_perm
      = (fs::perms::group_exec | fs::perms::owner_exec | fs::perms::others_exec);
    if ((st.permissions() & exec_perm) == fs::perms::none) {
      return false;
    }

    exec = std::move(dir);
    return true;
  };

  process_PATH(check_exec);

  if (!exec.empty()) {
    std::print("{} is {}", name, exec.c_str());
    return;
  }
  
  std::print("{}: not found", name);
}

void process_PATH(std::function<bool(std::filesystem::path&&)> action) {
  std::string_view var = std::getenv("PATH");

  for(; !var.empty();) {
    auto pos = var.find(PATH_separator());

    bool ret = action(
      std::filesystem::path(var.substr(0, pos))
    );

    if (ret) {
      return;
    }
    
    var.remove_prefix(pos);
  }
}

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  for(;;) {
    std::print("$ ");

    std::string command;
    std::getline(std::cin, command);
    std::istringstream stream(std::move(command));

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
