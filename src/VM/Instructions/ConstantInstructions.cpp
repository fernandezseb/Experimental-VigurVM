#include "ConstantInstructions.h"

#include <bit>

#include "Data/Variable.h"
#include "VM/VM.h"

void nop(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
}

void aconst_null(uint8_t* args, uint16_t argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    constexpr Variable reference{VariableType_REFERENCE};
    thread->currentFrame->operands.push_back(reference);
}

void iconst_i(uint8_t* args, uint16_t argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    const Variable variable{VariableType_INT,
        std::bit_cast<u4>(static_cast<int32_t>(arg))};
    thread->currentFrame->operands.push_back(variable);
}

void lconst_i(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    constexpr Variable variableHigh{VariableType_LONG};
    const Variable variableLow{VariableType_LONG,
        std::bit_cast<u4>(static_cast<int32_t>(arg))};
    thread->currentFrame->operands.push_back(variableHigh);
    thread->currentFrame->operands.push_back(variableLow);
}

void fconst_i(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    const float f = arg;
    const Variable variable{VariableType_FLOAT, std::bit_cast<u4>(f)};
    thread->currentFrame->operands.push_back(variable);
}

void dconst_i(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    double d = arg;
    const u4 lowBytes =  *((u8*)(&d));
    const u4 highBytes = (*((u8*)(&(d))) >> 32);

    // /uint64_t bytes = ((uint64_t)highBytes << 32) + (uint64_t)lowBytes;
    // The value is double back = *reinterpret_cast<double*> (&bytes);
    const Variable variableHigh{VariableType_LONG, highBytes};
    const Variable variableLow{VariableType_LONG, lowBytes};
    thread->currentFrame->operands.push_back(variableHigh);
    thread->currentFrame->operands.push_back(variableLow);
}

void bipush(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    const uint8_t byte = args[0];
    const Variable variable{VariableType_INT, byte};
    thread->currentFrame->operands.push_back(variable);
}

void sipush(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    const i2 shortValue = (args[0] << 8) | args[1];
    const i4 intValue = shortValue;
    Variable variable{VariableType_INT, static_cast<uint32_t>(intValue)};
    thread->currentFrame->operands.push_back(variable);
}

void loadConstant(const VMThread* thread, const u4 index, JavaHeap* heap, VM* VM)
{
    ConstantPoolItem* cpItem = thread->currentFrame->constantPool->constants[index-1];
    if (cpItem->getType() == CT_INTEGER)
    {
        const CPIntegerInfo* integerInfo = static_cast<CPIntegerInfo*>(cpItem);
        const Variable var{VariableType_INT, integerInfo->bytes};
        thread->currentFrame->operands.push_back(var);
    } else if (cpItem->getType() == CT_STRING)
    {
        CPStringInfo* stringInfo = (CPStringInfo*) cpItem;
        const char* utf8String = thread->currentClass->constantPool->getString(stringInfo->stringIndex);

        const u4 strObjectId = heap->createObject(heap->getClassByName("java/lang/String"), VM);
        const Object* strObject = heap->getObject(strObjectId);

        const u4 arrId = heap->createArray(AT_CHAR, strlen(utf8String));
        const Array* charArray = heap->getArray(arrId);
        memcpy((char*)charArray->data, utf8String, strlen(utf8String));

        const Variable var{VariableType_REFERENCE, arrId};
        strObject->fields[0].data[0] = var;

        const Variable strVar{VariableType_REFERENCE, strObjectId};
        thread->currentFrame->operands.push_back(strVar);
        // TODO: If type is 7, check if a Class reference has already been created,
        // TODO: if not, create this object and initialize the fields correctly
    } else
    {
        char buffer [200];
        snprintf(buffer, 200, "LDC not implemented yet for type: %d", cpItem->getType());
        thread->internalError(buffer);
    }
}

void loadConstant2(const VMThread* thread, const u4 index)
{
    ConstantPoolItem* cpItem = thread->currentFrame->constantPool->constants[index-1];
    if (cpItem->getType() == CT_LONG)
    {
        CPLongInfo* integerInfo = (CPLongInfo*) cpItem;
        Variable highVar{VariableType_LONG, integerInfo->highBytes};
        thread->currentFrame->operands.push_back(highVar);
        Variable lowVar{VariableType_LONG, integerInfo->lowBytes};
        thread->currentFrame->operands.push_back(lowVar);
    }
    else if (cpItem->getType() == CT_DOUBLE)
    {
        CPDoubleInfo* integerInfo = (CPDoubleInfo*) cpItem;
        Variable highVar{VariableType_DOUBLE, integerInfo->highBytes};
        thread->currentFrame->operands.push_back(highVar);
        Variable lowVar{VariableType_DOUBLE, integerInfo->lowBytes};

        thread->currentFrame->operands.push_back(lowVar);
    }
}

void ldc(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    u1 index = args[0];
    loadConstant(thread, index, heap, VM);
}

void ldc_w(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    u2 index = (args[0] << 8) | args[1];
    loadConstant(thread, index, heap, VM);
}

void ldc2_w(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    u2 index = (args[0] << 8) | args[1];
    loadConstant2(thread, index);
}
