#include "LoadInstructions.h"

#include "VM.h"
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
    Variable index = thread->currentFrame->popOperand();
    Variable arrayRef = thread->currentFrame->popOperand();

    Array* array = heap->getArray(arrayRef.data);
    i4* intArray = (i4*) array->data;

    i4 data = intArray[index.data];

    Variable dataVar = {};
    dataVar.type = VariableType_INT;
    dataVar.data = data;
    thread->currentFrame->operands.push_back(dataVar);
}

void caload(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    Variable index = thread->currentFrame->popOperand();
    Variable arrayRef = thread->currentFrame->popOperand();

    Array* array = heap->getArray(arrayRef.data);
    i1* intArray = (i1*) array->data;

    i1 data = intArray[index.data];

    Variable dataVar = {};
    dataVar.type = VariableType_INT;
    dataVar.data = (i4)data;
    thread->currentFrame->operands.push_back(dataVar);
}
