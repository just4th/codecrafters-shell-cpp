#include "builtins.hpp"

#include "filesystem_helpers.hpp"

#include <print>

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