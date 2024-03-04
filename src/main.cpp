/*
 * Copyright (c) 2023-2024 Sebastiaan Fernandez.
 *
 * This file is part of VigurVM.
 *
 * VigurVM is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * VigurVM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with Foobar.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "VM/Configuration.h"
#include "VM/VM.h"

#include <span>
#include <string_view>

static Configuration parseArguments(const std::span<const char*> args)
{
    Configuration config;
    bool mainNameFound = false;
    for (int currentArg = 1; currentArg < args.size(); ++currentArg)
    {
        const std::string_view arg{args[currentArg]};
        if (arg  == "-classpath")
        {
            if (currentArg+1 < args.size())
            {
                config.classPath = std::string_view{args[currentArg+1]};
                currentArg++;
            }
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

int main(const int argc, const char* argv[])
{
    const size_t size = argc;
    const std::span args{argv, size};
    const Configuration config = parseArguments(args);

    VM vm(config);
    vm.start(argv[0]);
    vm.runMain();
    vm.shutdown();

    return 0;
}
