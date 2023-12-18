#include "ExtendedInstructions.h"

#include "VM.h"
#include "Data/Variable.h"

static u2 readShort(VMThread* thread)
{
    uint8_t indexByte1 = thread->currentMethod->code->code[thread->pc++];
    uint8_t indexByte2 = thread->currentMethod->code->code[thread->pc++];
    uint16_t shortCombined = (indexByte1 << 8) | indexByte2;
    return shortCombined;
}

void ifnonnull(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    u2 branchByte = readShort(thread);
    // uint8_t byte = thread->currentMethod->code->code[thread->pc-3+branchByte];
    Variable ref = thread->currentFrame->popOperand();
    if (ref.data != 0) {
        thread->pc = thread->pc-3+branchByte;
    }
}
