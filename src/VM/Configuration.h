#pragma once

#include <string_view>
#include <vector>
#include <span>

struct Configuration
{
    std::string_view classPath;
    std::string_view mainClassName;
    std::vector<std::string_view> args;
    bool disableSystemInit = false;

    static Configuration fromArguments(const std::span<const char*> cargs)
    {
        Configuration config;
        bool mainNameFound = false;
        for (int currentArg = 1; currentArg < cargs.size(); ++currentArg)
        {
            const std::string_view arg{cargs[currentArg]};
            if (arg  == "-classpath")
            {
                if (currentArg+1 < cargs.size())
                {
                    config.classPath = std::string_view{cargs[currentArg+1]};
                    currentArg++;
                }
            } else if (arg == "-disableSystemInit")
            {
                config.disableSystemInit = true;
            }
            else
            {
                if (!mainNameFound)
                {
                    config.mainClassName = arg;
                    mainNameFound = true;
                } else
                {
                    config.args.push_back(arg);
                }
            }
        }

        return config;
    }
};
