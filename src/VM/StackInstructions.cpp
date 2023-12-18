#include "StackInstructions.h"

#include "VM.h"
#include "Data/Variable.h"

void pop(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    thread->currentFrame->popOperand();
}

void dup(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    Variable top = thread->currentFrame->peekOperand();
    Variable copy = {};
    copy.type = top.type;
    copy.data = top.data;
    thread->currentFrame->operands.push_back(copy);
}
