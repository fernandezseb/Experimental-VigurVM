#include "VM/Configuration.h"
#include "VM/VM.h"

#include <span>

Configuration parseArguments(const std::span<const char*> args)
{
    Configuration config;
    bool mainNameFound = false;
    for (int currentArg = 1; currentArg < args.size(); ++currentArg)
    {
        if (strcmp(args[currentArg], "-classpath") == 0 )
        {
            if (currentArg+1 < args.size())
            {
                config.classPath = args[currentArg+1];
                currentArg++;
            }
        }
        else
        {
            if (!mainNameFound)
            {
                config.mainClassName = args[currentArg];
                mainNameFound = true;
            } else
            {
                config.args.push_back(args[currentArg]);
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
    vm.runMain(config.mainClassName);
    vm.shutdown();

    return 0;
}
