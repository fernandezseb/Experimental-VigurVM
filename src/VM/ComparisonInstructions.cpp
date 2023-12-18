#include "ComparisonInstructions.h"

#include "VM.h"
#include "Data/Variable.h"

void ifge(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    u1 byte1 = args[0];
    u1 byte2 = args[1];

    u2 branchByte = (byte1 << 8) | byte2;


    // TODO: Check if it is int
    Variable intVar = thread->currentFrame->popOperand();
    if (((i4)intVar.data) >= 0)
    {
        thread->pc = thread->pc-3+branchByte;
    }
}

void ifle(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    u1 byte1 = args[0];
    u1 byte2 = args[1];

    u2 branchByte = (byte1 << 8) | byte2;


    // TODO: Check if it is int
    Variable intVar = thread->currentFrame->popOperand();
    if (((i4)intVar.data) <= 0)
    {
        thread->pc = thread->pc-3+branchByte;
    }
}
