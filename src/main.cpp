#include <iostream>
#include <sstream>
#include <string>
#include <print>
#include <map>
#include <functional>
#include <filesystem>
#include <unistd.h>
#include <sys/wait.h>


consteval char PATH_separator() {
#ifdef __WIN32
  return ';';
#else
  return ':';
#endif
}

bool execute_command(std::filesystem::path&& exec, std::vector<std::string>&& args) {
#ifdef __WIN32
  //TODO
#else
  auto pid = fork();
  if (pid < 0) {
    return false;
  } else if (pid > 0) {

    int wstatus;
    waitpid(pid, &wstatus, 0);
    std::ignore = wstatus;
    return true;
  }

  std::vector<char*> raw_args;
  raw_args.reserve(args.size() + 1);
  for (auto& arg : args) {
    raw_args.push_back(arg.data());
  }
  raw_args.push_back(nullptr);

  execv(exec.c_str(), raw_args.data());
  std::abort();

#endif
}

using CommandHandler = std::function<void(std::vector<std::string>&&)>;

void process_PATH(std::function<bool(std::filesystem::path&&)>);
void EchoHandler(std::vector<std::string>&&);
void TypeHandler(std::vector<std::string>&&);
void PwdHandler(std::vector<std::string>&&);
void CdHandler(std::vector<std::string>&&);

const std::unordered_map<std::string, const CommandHandler> HANDLERS = {
  {"exit", {}},
  {"echo", EchoHandler},
  {"type", TypeHandler},
  {"pwd", PwdHandler},
  {"cd", CdHandler},
};

std::filesystem::path find_exec(const std::string_view name) {
  namespace fs = std::filesystem;

  fs::path res;
  auto impl = [&res, &name](fs::path&& dir) {
    dir.append(name);
    auto st = fs::status(dir);
    if (!fs::is_regular_file(st)) {
      return false;
    }

    auto exec_perm
      = (fs::perms::group_exec | fs::perms::owner_exec | fs::perms::others_exec);
    if ((st.permissions() & exec_perm) == fs::perms::none) {
      return false;
    }

    res = std::move(dir);
    return true;
  };

  process_PATH(impl);

  return res;
}

void EchoHandler(std::vector<std::string>&& args) {
  if (args.size() < 2) {
    std::println();
    return;
  }
  std::print("{}", args[1]);
  for (auto it = args.begin() + 2; it != args.end(); ++it) {
    std::print(" {}", *it);
  }
  std::println();
}

void TypeHandler(std::vector<std::string>&& args) {
  if (args.size() < 2) {
    return;
  }
  const auto name = args[1];

  if (HANDLERS.contains(name)) {
    std::println("{} is a shell builtin", name);
    return;
  }

  const auto exec = find_exec(name);
  if (!exec.empty()) {
    std::println("{} is {}", name, exec.c_str());
    return;
  }

  std::println("{}: not found", name);
}

void PwdHandler(std::vector<std::string>&&) {
  std::println("{}", std::filesystem::current_path().c_str());
}

void CdHandler(std::vector<std::string>&& args) {
  namespace fs = std::filesystem;
  if (args.size() < 2) {
    return;
  }
  auto& path_string = args[1];
  
  if (path_string[0] == '~') {
    path_string = std::getenv("HOME") + path_string.substr(1);
  }
  fs::path dir(std::move(path_string));
  auto st = fs::status(dir);
  if (!fs::exists(st)) {
    std::println("{}: No such file or directory", dir.c_str());
    return;
  }
  if (!fs::is_directory(st)) {
    std::println("{}: Not a directory", dir.c_str());
  }

  std::filesystem::current_path(dir);
}

void process_PATH(std::function<bool(std::filesystem::path&&)> action) {
  std::string_view var = std::getenv("PATH");

  for(; !var.empty();) {
    auto pos = var.find(PATH_separator());

    if (pos == var.npos) {
      pos = var.size();
    }

    bool ret = action(
      std::filesystem::path(var.substr(0, pos))
    );

    if (ret) {
      return;
    }
    if (pos == var.size()) {
      break;
    }

    var.remove_prefix(pos + 1);
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

    std::vector<std::string> args;
    for (std::string arg; stream >> arg;) {
      args.push_back(std::move(arg));
    }

    if (args.empty()) {
      continue;
    }

    if (args[0] == "exit") {
      break;
    }

    auto it = HANDLERS.find(args[0]);
    if (it != HANDLERS.end()) {
      it->second(std::move(args));
      continue;
    }

    auto exec = find_exec(args[0]);
    if (exec.empty()) {
      println("{}: command not found", args[0]);
      continue; 
    }

    execute_command(std::move(exec), std::move(args));
  }

  return 0;
}
