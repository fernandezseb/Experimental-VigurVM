
#pragma once
#include <string_view>
#include <vector>

struct Configuration
{
    std::string_view classPath;
    std::string_view mainClassName;
    std::vector<std::string_view> args;
};
