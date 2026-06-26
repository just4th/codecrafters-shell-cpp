#pragma once

#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <functional>


void EchoHandler(std::vector<std::string>&&);

void TypeHandler(std::vector<std::string>&&);

void PwdHandler(std::vector<std::string>&&);

void CdHandler(std::vector<std::string>&&);


const std::unordered_map<std::string, std::function<void(std::vector<std::string>&&)>> HANDLERS = {
    {"exit", {}},
    {"echo", EchoHandler},
    {"type", TypeHandler},
    {"pwd", PwdHandler},
    {"cd", CdHandler},
};
