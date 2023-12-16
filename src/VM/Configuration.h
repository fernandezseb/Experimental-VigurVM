
#pragma once
#include <vector>

struct Configuration
{
    const char* classPath = 0;
    const char* mainClassName = 0;
    std::vector<const char*> args;
};
