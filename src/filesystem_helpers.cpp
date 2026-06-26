#include "filesystem_helpers.hpp"

namespace {
    
    consteval char PATH_separator() {
    #ifdef __WIN32
        return ';';
    #else
        return ':';
    #endif
    }

}

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