#include "ConstantInstructions.h"

#include "Data/Variable.h"
#include "VM.h"

void aconst_null(uint8_t* args, uint16_t argsCount, i1 arg, JavaHeap* heap, VMThread* thread)
{
    Variable reference = {};
    reference.type = VariableType_REFERENCE;
    reference.data = 0;
    thread->currentFrame->operands.push_back(reference);
}

void iconst_i(uint8_t* args, uint16_t argsCount, i1 arg, JavaHeap* heap, VMThread* thread)
{
    Variable variable;
    variable.type = VariableType_INT;
    variable.data = ((int32_t)arg);
    thread->currentFrame->operands.push_back(variable);
}

void lconst_i(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread)
{
    Variable variableHigh = {};
    variableHigh.type = VariableType_LONG;
    variableHigh.data = ((int32_t)0);
    Variable variableLow = {};
    variableLow.type = VariableType_LONG;
    variableLow.data = ((int32_t)arg);
    thread->currentFrame->operands.push_back(variableHigh);
    thread->currentFrame->operands.push_back(variableLow);
}

void fconst_i(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread)
{
    Variable variable = {};
    variable.type = VariableType_FLOAT;
    float f = arg;
    u4 casted = *((u4*)(&f));
    variable.data = (casted);
    thread->currentFrame->operands.push_back(variable);
}