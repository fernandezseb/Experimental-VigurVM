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
    vm.start();
    vm.runMain();
    vm.shutdown();

    return 0;
}
