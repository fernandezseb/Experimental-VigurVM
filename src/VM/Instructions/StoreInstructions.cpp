#include "StoreInstructions.h"

#include "VM/VM.h"
#include "Data/Variable.h"

void istore(const uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    const Variable refVar = thread->currentFrame->popOperand();
    const u1 index = args[0];
    thread->currentFrame->localVariables[index] = refVar;
}

void astore(const uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    const Variable refVar = thread->currentFrame->popOperand();
    const u1 index = args[0];
    thread->currentFrame->localVariables[index] = refVar;
}

void istore_i(const uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    const Variable refVar = thread->currentFrame->popOperand();
    VM::checkType(refVar, VariableType_INT, thread);
    thread->currentFrame->localVariables[arg] = refVar;
}

void astore_i(const uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    const Variable refVar = thread->currentFrame->popOperand();
    thread->currentFrame->localVariables[arg] = refVar;
}

void iastore(const uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    Variable value = thread->currentFrame->popOperand();
    const Variable index = thread->currentFrame->popOperand();
    const Variable arrayref = thread->currentFrame->popOperand();

    const Array* array = heap->getArray(arrayref.data);
    i4* intArray = reinterpret_cast<i4*>(array->data);
    intArray[index.data] = *reinterpret_cast<i4*>(&value.data);
}

void aastore(const uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
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

void castore(const uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    StackFrame* currentFrame = thread->currentFrame;
    const Variable value = currentFrame->popOperand();
    const Variable index = currentFrame->popOperand();
    const Variable arrayref = currentFrame->popOperand();

    const Array* array = heap->getArray(arrayref.data);
    array->data[index.data] = value.data;
}
