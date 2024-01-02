#include "LoadInstructions.h"

#include <bit>

#include "VM/VM.h"
#include "Data/Variable.h"


void iload(INSTRUCTION_ARGS)
{
    u1 index = args[0];
    Variable var = thread->m_currentFrame->localVariables[index];
    VM::checkType(var, VariableType_INT, thread);

    thread->m_currentFrame->operands.push_back(var);
}

void aload_i(INSTRUCTION_ARGS)
{
    Variable var = thread->m_currentFrame->localVariables[arg];
    VM::checkType(var, VariableType_REFERENCE, thread);

    thread->m_currentFrame->operands.push_back(var);
}

void iload_i(INSTRUCTION_ARGS)
{
    Variable var = thread->m_currentFrame->localVariables[arg];
    VM::checkType(var, VariableType_INT, thread);

    thread->m_currentFrame->operands.push_back(var);
}

void iaload(INSTRUCTION_ARGS)
{
    const Variable index = thread->m_currentFrame->popOperand();
    const Variable arrayRef = thread->m_currentFrame->popOperand();

    const Array* array = heap->getArray(arrayRef.data);
    const i4* intArray = (i4*) array->data;

    const i4 data = intArray[index.data];

    const Variable dataVar{VariableType_INT, std::bit_cast<u4>(data)};
    thread->m_currentFrame->operands.push_back(dataVar);
}

void caload(INSTRUCTION_ARGS)
{
    const Variable index = thread->m_currentFrame->popOperand();
    const Variable arrayRef = thread->m_currentFrame->popOperand();

    const Array* array = heap->getArray(arrayRef.data);
    const i1* intArray = (i1*) array->data;

    const i1 data = intArray[index.data];

    Variable dataVar{VariableType_INT, static_cast<uint32_t>(static_cast<int32_t>(data))};
    thread->m_currentFrame->operands.push_back(dataVar);
}
