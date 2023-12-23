#include "VM/Configuration.h"
#include "VM/VM.h"

Configuration parseArguments(const int argc, const char*const*const argv)
{
    Configuration config;
    bool mainNameFound = false;
    for (int currentArg = 1; currentArg < argc; ++currentArg)
    {
        if (strcmp(argv[currentArg], "-classpath") == 0 )
        {
            if (currentArg+1 < argc)
            {
                config.classPath = argv[currentArg+1];
                currentArg++;
            }
        }
        else
        {
            if (!mainNameFound)
            {
                config.mainClassName = argv[currentArg];
                mainNameFound = true;
            } else
            {
                config.args.push_back(argv[currentArg]);
            }
        }
    }

    return config;
}

int main(const int argc, const char*const*const argv)
{
    const Configuration config = parseArguments(argc, argv);

    VM vm(config);
    vm.start();
    vm.runMain(config.mainClassName);
    vm.shutdown();

    return 0;
}
