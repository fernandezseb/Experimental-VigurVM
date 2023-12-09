
#include "VM/VM.h"

int main(int argc, char* argv[])
{
    VM vm;
    vm.start();
    vm.runMain("Main");
    return 0;
}
