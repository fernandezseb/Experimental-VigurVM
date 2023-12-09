
#include "VM/Configuration.h"
#include "VM/VM.h"

int main(int argc, char* argv[])
{
    VM vm;
    Configuration config;
    config.classPath = "C:/Users/Sebastiaan/.jdks/temurin-1.8.0_392/jre/lib/rt/";
    vm.start(config);
    vm.runMain("Main");
    return 0;
}
