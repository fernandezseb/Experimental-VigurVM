#include "StoreInstructions.h"

#include "Util.h"
#include "VM/VM.h"
#include "Data/Variable.h"

void istore(INSTRUCTION_ARGS)
{
    const Variable refVar = thread->currentFrame->popOperand();
    const u1 index = args[0];
    thread->currentFrame->localVariables[index] = refVar;
}

void astore(INSTRUCTION_ARGS)
{
    const Variable refVar = thread->currentFrame->popOperand();
    const u1 index = args[0];
    thread->currentFrame->localVariables[index] = refVar;
}

void istore_i(INSTRUCTION_ARGS)
{
    const Variable refVar = thread->currentFrame->popOperand();
    VM::checkType(refVar, VariableType_INT, thread);
    thread->currentFrame->localVariables[arg] = refVar;
}

void astore_i(INSTRUCTION_ARGS)
{
    const Variable refVar = thread->currentFrame->popOperand();
    thread->currentFrame->localVariables[arg] = refVar;
}

void iastore(INSTRUCTION_ARGS)
{
    Variable value = thread->currentFrame->popOperand();
    const Variable index = thread->currentFrame->popOperand();
    const Variable arrayref = thread->currentFrame->popOperand();

    const Array* array = heap->getArray(arrayref.data);
    i4* intArray = reinterpret_cast<i4*>(array->data);
    intArray[index.data] = *reinterpret_cast<i4*>(&value.data);
}

void aastore(INSTRUCTION_ARGS)
{
    StackFrame* currentFrame = thread->currentFrame;
    const Variable value = currentFrame->popOperand();
    const Variable index = currentFrame->popOperand();
    const Variable arrayRef = currentFrame->popOperand();

    VM::checkType(value, VariableType_REFERENCE, thread);
    VM::checkType(index, VariableType_INT, thread);
    VM::checkType(arrayRef, VariableType_REFERENCE, thread);

    const Array* arrayArr = heap->getArray(arrayRef.data);

    if (arrayArr->arrayType != AT_REFERENCE)
    {
        thread->internalError("Array type must be REFERENCE");
    }

    auto* arrData = reinterpret_cast<uint32_t*>(arrayArr->data);
    arrData[index.data] = value.data;
}

void castore(INSTRUCTION_ARGS)
{
    StackFrame* currentFrame = thread->currentFrame;
    const Variable value = currentFrame->popOperand();
    const Variable index = currentFrame->popOperand();
    const Variable arrayref = currentFrame->popOperand();

    const Array* array = heap->getArray(arrayref.data);
    array->data[index.data] = castToU1<u4>(value.data);
}
