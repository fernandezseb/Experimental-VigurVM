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

void dup_x1(INSTRUCTION_ARGS) {
    const Variable value1 = thread->m_currentFrame->popOperand();
    const Variable value2 = thread->m_currentFrame->popOperand();
    thread->m_currentFrame->operands.push_back(value1);
    thread->m_currentFrame->operands.push_back(value2);
    thread->m_currentFrame->operands.push_back(value1);
}
