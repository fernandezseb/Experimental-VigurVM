
#include <execution>

#include "VM/Configuration.h"
#include "VM/VM.h"


Configuration parseArguments(int argc, char** argv)
{
    Configuration config;

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
            config.mainClassName = argv[currentArg];
        }
    }

    return config;
}

int main(int argc, char* argv[])
{
    VM vm;

    Configuration config = parseArguments(argc, argv);

    vm.start(config);
    vm.runMain(config.mainClassName);
    return 0;
}
