#include "StoreInstructions.h"

#include "VM.h"
#include "Data/Variable.h"

void istore(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    Variable refVar = thread->currentFrame->popOperand();
    u1 index = args[0];
    Variable* var =  &thread->currentFrame->localVariables[index];
    var->data = refVar.data;
    var->type = refVar.type;
}

void istore_i(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    // TODO: Check types
    Variable refVar = thread->currentFrame->popOperand();
    Variable* var =  &thread->currentFrame->localVariables[arg];
    var->data = refVar.data;
    var->type = refVar.type;
}

void astore_i(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    Variable refVar = thread->currentFrame->popOperand();
    Variable* var =  &thread->currentFrame->localVariables[arg];
    var->data = refVar.data;
    var->type = refVar.type;
}

void iastore(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    Variable value = thread->currentFrame->popOperand();
    Variable index = thread->currentFrame->popOperand();
    Variable arrayref = thread->currentFrame->popOperand();

    Array* array = heap->getArray(arrayref.data);
    i4* intArray = (i4*) array->data;
    intArray[index.data] = (i4) value.data;
}

void castore(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    Variable value = thread->currentFrame->popOperand();
    Variable index = thread->currentFrame->popOperand();
    Variable arrayref = thread->currentFrame->popOperand();

    Array* array = heap->getArray(arrayref.data);
    array->data[index.data] = value.data;
}
