#include "StackInstructions.h"

#include "VM/VM.h"
#include "Data/Variable.h"

void pop(const uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    thread->currentFrame->popOperand();
}

void dup(const uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    const Variable top = thread->currentFrame->popOperand();
    thread->currentFrame->operands.push_back(top);
    thread->currentFrame->operands.push_back(top);
}
