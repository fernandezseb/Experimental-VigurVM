#pragma once

#include <string_view>
#include <vector>
#include <span>

struct Configuration
{
    std::string_view m_classPath;
    std::string_view m_mainClassName;
    std::vector<std::string_view> m_args;
    bool m_disableSystemInit = false;

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
                    config.m_classPath = std::string_view{cargs[currentArg+1]};
                    currentArg++;
                }
            } else if (arg == "-disableSystemInit")
            {
                config.m_disableSystemInit = true;
            }
            else
            {
                if (!mainNameFound)
                {
                    config.m_mainClassName = arg;
                    mainNameFound = true;
                } else
                {
                    config.m_args.push_back(arg);
                }
            }
        }

        return config;
    }
};
