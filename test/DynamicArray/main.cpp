#include "DynamicArray.h"

int main(int argc, char* argv[])
{
    DynamicArray<uint32_t> intArray(5);
    intArray.add(2);
    intArray.add(3);
    intArray.add(2);
    intArray.add(4);
    intArray.add(2);
    intArray.add(3);
    if (intArray.getSize() != 6)
    {
        return -1;
    }

    if (intArray.get(0) != 2)
    {
        return -2;
    }

    if (intArray.get(1) != 3)
    {
        return -3;
    }
    if (intArray.get(5) != 3)
    {
        return -4;
    }
    if (intArray.set(6, 10) >= 0)
    {
        return -4;
    }
    return 0;
}
