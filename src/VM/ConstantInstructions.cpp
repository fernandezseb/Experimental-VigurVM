#include "ConstantInstructions.h"

#include "Data/Variable.h"
#include "VM.h"

void nop(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread)
{
}

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

void dconst_i(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread)
{
    double d = arg;
    u4 lowBytes =  *((u8*)(&d));
    u4 highBytes = (*((u8*)(&(d))) >> 32);

    uint64_t bytes = ((uint64_t)highBytes << 32) + (uint64_t)lowBytes;
    // The value is double back = *reinterpret_cast<double*> (&bytes);
    Variable variableHigh = {};
    variableHigh.type = VariableType_LONG;
    variableHigh.data = highBytes;
    Variable variableLow = {};
    variableLow.type = VariableType_LONG;
    variableLow.data = lowBytes;
    thread->currentFrame->operands.push_back(variableHigh);
    thread->currentFrame->operands.push_back(variableLow);
}

void bipush(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread)
{
    uint8_t byte = args[0];
    Variable variable = {};
    variable.type = VariableType_INT;
    variable.data = byte;
    thread->currentFrame->operands.push_back(variable);
}

void sipush(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread)
{
    i2 shortValue = (args[0] << 8) | args[1];
    i4 intValue = shortValue;
    Variable variable = {};
    variable.type = VariableType_INT;
    variable.data = intValue;
    thread->currentFrame->operands.push_back(variable);
}

void loadConstant(VMThread* thread, u4 index, JavaHeap* heap)
{
    ConstantPoolItem* cpItem = thread->currentFrame->constantPool->constants[index-1];
    if (cpItem->getType() == CT_INTEGER)
    {
        CPIntegerInfo* integerInfo = (CPIntegerInfo*) cpItem;
        Variable var = {};
        var.type = VariableType_INT;
        var.data = integerInfo->bytes;
        thread->currentFrame->operands.push_back(var);
    } else if (cpItem->getType() == CT_STRING)
    {
        CPStringInfo* stringInfo = (CPStringInfo*) cpItem;
        const char* utf8String = thread->currentClass->constantPool->getString(stringInfo->stringIndex);

        u4 strObjectId = heap->createObject(heap->getClassByName("java/lang/String"));
        Object* strObject = heap->getObject(strObjectId);

        u4 arrId = heap->createArray(AT_CHAR, strlen(utf8String));
        Array* charArray = heap->getArray(arrId);
        memcpy((char*)charArray->data, utf8String, strlen(utf8String));

        Variable var = {};
        var.type = VariableType_REFERENCE;
        var.data = arrId;
        strObject->fields[0].data = var;

        Variable strVar = {};
        strVar.type = VariableType_REFERENCE;
        strVar.data = strObjectId;
        thread->currentFrame->operands.push_back(strVar);
    } else
    {
        fprintf(stderr, "Error: LDC not implemented yet for type: %d\n", cpItem->getType());
        Platform::exitProgram(-32);
    }
}

void loadConstant2(VMThread* thread, u4 index)
{
    ConstantPoolItem* cpItem = thread->currentFrame->constantPool->constants[index-1];
    if (cpItem->getType() == CT_LONG)
    {
        CPLongInfo* integerInfo = (CPLongInfo*) cpItem;
        Variable highVar = {};
        highVar.type = VariableType_LONG;
        highVar.data = integerInfo->highBytes;
        thread->currentFrame->operands.push_back(highVar);
        Variable lowVar = {};
        lowVar.type = VariableType_LONG;
        lowVar.data = integerInfo->lowBytes;
        thread->currentFrame->operands.push_back(lowVar);
    }
    else if (cpItem->getType() == CT_DOUBLE)
    {
        CPDoubleInfo* integerInfo = (CPDoubleInfo*) cpItem;
        Variable highVar = {};
        highVar.type = VariableType_DOUBLE;
        highVar.data = integerInfo->highBytes;
        thread->currentFrame->operands.push_back(highVar);
        Variable lowVar = {};
        lowVar.type = VariableType_DOUBLE;
        lowVar.data = integerInfo->lowBytes;
        thread->currentFrame->operands.push_back(lowVar);
    }
}

void ldc(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread)
{
    u1 index = args[0];
    loadConstant(thread, index, heap);
}

void ldc_w(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread)
{
    u2 index = (args[0] << 8) | args[1];
    loadConstant(thread, index, heap);
}

void ldc2_w(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread)
{
    u2 index = (args[0] << 8) | args[1];
    loadConstant2(thread, index);
}
