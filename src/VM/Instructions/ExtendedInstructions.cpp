#include "ExtendedInstructions.h"

#include "VM/VM.h"
#include "Data/Variable.h"

static u2 readShort(VMThread* thread)
{
    uint8_t indexByte1 = thread->m_currentMethod->code->code[thread->m_pc++];
    uint8_t indexByte2 = thread->m_currentMethod->code->code[thread->m_pc++];
    uint16_t shortCombined = (indexByte1 << 8) | indexByte2;
    return shortCombined;
}

void ifnull(INSTRUCTION_ARGS)
{
    const u2 branchByte = readShort(thread);
    // uint8_t byte = thread->currentMethod->code->code[thread->pc-3+branchByte];
    const Variable ref = thread->m_currentFrame->popOperand();
    if (ref.data == 0) {
        thread->m_pc = thread->m_pc-3+branchByte;
    }
}

void ifnonnull(INSTRUCTION_ARGS)
{
    const u2 branchByte = readShort(thread);
    // uint8_t byte = thread->currentMethod->code->code[thread->pc-3+branchByte];
    const Variable ref = thread->m_currentFrame->popOperand();
    if (ref.data != 0) {
        thread->m_pc = thread->m_pc-3+branchByte;
    }
}
