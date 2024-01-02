#include "StackInstructions.h"

#include "VM/VM.h"
#include "Data/Variable.h"

void pop(INSTRUCTION_ARGS)
{
    thread->m_currentFrame->popOperand();
}

void dup(INSTRUCTION_ARGS)
{
    const Variable top = thread->m_currentFrame->popOperand();
    thread->m_currentFrame->operands.push_back(top);
    thread->m_currentFrame->operands.push_back(top);
}
