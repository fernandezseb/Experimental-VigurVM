#include "LoadInstructions.h"

#include "VM.h"
#include "Data/Variable.h"


void aload_i(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    Variable var = thread->currentFrame->localVariables[arg];
    thread->currentFrame->operands.push_back(var);
}

void iload_i(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    Variable var = thread->currentFrame->localVariables[arg];
    thread->currentFrame->operands.push_back(var);
}
