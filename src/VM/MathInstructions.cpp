#include "MathInstructions.h"

#include "VM.h"
#include "Data/Variable.h"

void iadd(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    // TODO: Check types and convert to int correctly
    Variable var1 = thread->currentFrame->popOperand();
    Variable var2 = thread->currentFrame->popOperand();
    Variable added = {};
    added.type = VariableType_INT;
    added.data = (i4)var1.data + (i4)var2.data;
    thread->currentFrame->operands.push_back(added);
}

void isub(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    // TODO: Check types and convert to int correctly
    Variable var2 = thread->currentFrame->popOperand();
    Variable var1 = thread->currentFrame->popOperand();
    Variable added = {};
    added.type = VariableType_INT;
    added.data = (i4)var1.data - (i4)var2.data;
    thread->currentFrame->operands.push_back(added);
}
