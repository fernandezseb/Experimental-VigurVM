#include "StackInstructions.h"

#include "VM.h"
#include "Data/Variable.h"

void pop(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    thread->currentFrame->popOperand();
}

void dup(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    const Variable top = thread->currentFrame->popOperand();
    thread->currentFrame->operands.push_back(top);
    thread->currentFrame->operands.push_back(top);
}
