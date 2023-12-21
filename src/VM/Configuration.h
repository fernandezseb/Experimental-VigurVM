
#pragma once
#include <vector>

struct Configuration
{
    const char* classPath = nullptr;
    const char* mainClassName = nullptr;
    std::vector<const char*> args;
};
