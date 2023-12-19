#include "Core.h"
#include "ClassLoader/ClassLoader.h"
#include "Printer/ClassPrinter.h"
#include "ClassDumpConfig.h"

int main(int argc, char* argv[])
{
    Platform::initialize();
    if (argc > 1) {
        if (strncmp(argv[1], "-v", 2) == 0) {
            printf("ClassDump version %d.%d\n", CLASSDUMP_VERSION_MAJOR, CLASSDUMP_VERSION_MINOR);
        } else {
            Memory memory(1000, 5 * 1024 * 1024);
            const char* className = argv[1];

            ClassLoader classLoader;
            ClassPrinter classPrinter;
            ClassInfo* classInfo = classLoader.readClass(className, &memory);
            classPrinter.printClass(*classInfo, &memory);
#ifndef NDEBUG
            memory.printSize();
#endif
        }
    } else {
        fprintf(stderr, "Error: Class name must be supplied\n");
        Platform::exitProgram(1);
    }
    Platform::cleanup();

    return 0;
}
