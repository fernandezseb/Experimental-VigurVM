#include "StackInstructions.h"

#include "VM/VM.h"
#include "Data/Variable.h"

void pop(INSTRUCTION_ARGS)
{
    thread->currentFrame->popOperand();
}

void dup(INSTRUCTION_ARGS)
{
    const Variable top = thread->currentFrame->popOperand();
    thread->currentFrame->operands.push_back(top);
    thread->currentFrame->operands.push_back(top);
}
