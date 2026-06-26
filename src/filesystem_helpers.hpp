#pragma once

#include <filesystem>
#include <functional>

std::filesystem::path find_exec(const std::string_view name);

void process_PATH(std::function<bool(std::filesystem::path&&)> action);