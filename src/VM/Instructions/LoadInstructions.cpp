#include "LoadInstructions.h"

#include <bit>

#include "VM/VM.h"
#include "Data/Variable.h"


void iload(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    u1 index = args[0];
    Variable var = thread->currentFrame->localVariables[index];
    VM->checkType(var, VariableType_INT);

    thread->currentFrame->operands.push_back(var);
}

void aload_i(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    Variable var = thread->currentFrame->localVariables[arg];
    VM->checkType(var, VariableType_REFERENCE);

    thread->currentFrame->operands.push_back(var);
}

void iload_i(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    Variable var = thread->currentFrame->localVariables[arg];
    VM->checkType(var, VariableType_INT);

    thread->currentFrame->operands.push_back(var);
}

void iaload(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    const Variable index = thread->currentFrame->popOperand();
    const Variable arrayRef = thread->currentFrame->popOperand();

    const Array* array = heap->getArray(arrayRef.data);
    const i4* intArray = (i4*) array->data;

    const i4 data = intArray[index.data];

    const Variable dataVar{VariableType_INT, std::bit_cast<u4>(data)};
    thread->currentFrame->operands.push_back(dataVar);
}

void caload(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    const Variable index = thread->currentFrame->popOperand();
    const Variable arrayRef = thread->currentFrame->popOperand();

    const Array* array = heap->getArray(arrayRef.data);
    const i1* intArray = (i1*) array->data;

    const i1 data = intArray[index.data];

    Variable dataVar{VariableType_INT, static_cast<uint32_t>(static_cast<int32_t>(data))};
    thread->currentFrame->operands.push_back(dataVar);
}
