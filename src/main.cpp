#include "builtins.hpp"
#include "filesystem_helpers.hpp"

#include <iostream>
#include <print>

#ifdef __WIN32
    //TODO
#else

#include <unistd.h>
#include <sys/wait.h>

#endif


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

std::string get_arg(std::string_view& tail) {
    std::size_t prefix = 0;
    bool single_quotes = false;
    bool double_quotes = false;
    bool escape_char = false;
    std::string res;
    for (;prefix < tail.size(); ++prefix) {
        const auto ch = tail[prefix];
        if (escape_char) {
            escape_char = false;
            res += ch;
            continue;
        }

        if (ch == '\\') {
            escape_char = true;

            continue;
        }

        if (ch == '\'' && !double_quotes) {
            single_quotes = !single_quotes;
            continue;
        }

        if(ch == '"' && !single_quotes) {
            double_quotes = !double_quotes;
            continue;
        }

        if (!std::isspace(ch) || single_quotes || double_quotes) {
            res += ch;
            continue;
        } else if (!res.empty()) {
            break;
        }

    }

    tail.remove_prefix(prefix);
    return res;
}

int main() {
    // Flush after every std::cout / std:cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    for(;;) {
        std::print("$ ");

        std::string command;
        std::getline(std::cin, command);

        std::string_view tail(command);

        std::vector<std::string> args;
        while (!tail.empty()) {
            args.push_back(get_arg(tail));
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
