#include "ConstantInstructions.h"

#include "Data/Variable.h"
#include "VM/VM.h"

#include <bit>

void nop(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
}

void aconst_null(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    constexpr Variable reference{VariableType_REFERENCE};
    thread->currentFrame->operands.push_back(reference);
}

void iconst_i(const u1* args, u2 argsCount, const i1 arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    const Variable variable{VariableType_INT,
        std::bit_cast<u4>(static_cast<int32_t>(arg))};
    thread->currentFrame->operands.push_back(variable);
}

void lconst_i(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    constexpr Variable variableHigh{VariableType_LONG};
    const Variable variableLow{VariableType_LONG,
        std::bit_cast<u4>(static_cast<int32_t>(arg))};
    thread->currentFrame->operands.push_back(variableHigh);
    thread->currentFrame->operands.push_back(variableLow);
}

void fconst_i(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    const float f = arg;
    const Variable variable{VariableType_FLOAT, std::bit_cast<u4>(f)};
    thread->currentFrame->operands.push_back(variable);
}

void dconst_i(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    const double d = arg;
    const u4 lowBytes =  *((u8*)(&d));
    const u4 highBytes = (*((u8*)(&(d))) >> 32);

    // /uint64_t bytes = ((uint64_t)highBytes << 32) + (uint64_t)lowBytes;
    // The value is double back = *reinterpret_cast<double*> (&bytes);
    const Variable variableHigh{VariableType_LONG, highBytes};
    const Variable variableLow{VariableType_LONG, lowBytes};
    thread->currentFrame->operands.push_back(variableHigh);
    thread->currentFrame->operands.push_back(variableLow);
}

void bipush(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    const uint8_t byte = args[0];
    const Variable variable{VariableType_INT, byte};
    thread->currentFrame->operands.push_back(variable);
}

void sipush(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    const i2 shortValue = (args[0] << 8) | args[1];
    const i4 intValue = shortValue;
    const Variable variable{VariableType_INT, static_cast<uint32_t>(intValue)};
    thread->currentFrame->operands.push_back(variable);
}

void loadConstant(const VMThread* thread, const u4 index, JavaHeap* heap, VM* VM)
{
    const ConstantPoolItem* cpItem = thread->currentFrame->constantPool->constants[index - 1];
    if (cpItem->getType() == CT_INTEGER)
    {
        const CPIntegerInfo* integerInfo = static_cast<const CPIntegerInfo*>(cpItem);
        const Variable var{VariableType_INT, integerInfo->bytes};
        thread->currentFrame->operands.push_back(var);
    }
    else if (cpItem->getType() == CT_FLOAT)
    {
        const CPFloatInfo* floatInfo = static_cast<const CPFloatInfo*>(cpItem);
        const Variable var{VariableType_FLOAT, floatInfo->bytes};
        thread->currentFrame->operands.push_back(var);
    }
    else if (cpItem->getType() == CT_STRING)
    {
        const CPStringInfo* stringInfo = static_cast<const CPStringInfo*>(cpItem);
        const char* utf8String = thread->currentClass->constantPool->getString(stringInfo->stringIndex);
        const uint32_t strObjectId = heap->createString(utf8String, VM);
        const Variable strVar{VariableType_REFERENCE, strObjectId};
        thread->currentFrame->operands.push_back(strVar);
    }
    else if (cpItem->getType() == CT_CLASS)
    {
        const CPClassInfo* classInfo = static_cast<const CPClassInfo*>(cpItem);
        const char* className = thread->currentClass->constantPool->getString(classInfo->nameIndex);
        ClassInfo* targetClassInfo = VM->getClass(className, const_cast<VMThread*>(thread));
        const u4 classObjectRef =  heap->createClassObject(heap->getClassByName("java/lang/Class"), VM, targetClassInfo);
        // const ClassObject * classObject = heap->getClassObject(classObjectRef);
        const Variable classObjectVar{VariableType_REFERENCE, classObjectRef};
        thread->currentFrame->operands.push_back(classObjectVar);
    }
    else
    {
        char buffer[200];
        snprintf(buffer, 200, "LDC not implemented yet for type: %d", cpItem->getType());
        thread->internalError(buffer);
    }
}

void loadConstant2(const VMThread* thread, const u4 index)
{
    const ConstantPoolItem* cpItem = thread->currentFrame->constantPool->constants[index-1];
    if (cpItem->getType() == CT_LONG)
    {
        const CPLongInfo* integerInfo = (CPLongInfo*) cpItem;
        const Variable highVar{VariableType_LONG, integerInfo->highBytes};
        thread->currentFrame->operands.push_back(highVar);
        const Variable lowVar{VariableType_LONG, integerInfo->lowBytes};
        thread->currentFrame->operands.push_back(lowVar);
    }
    else if (cpItem->getType() == CT_DOUBLE)
    {
        const CPDoubleInfo* integerInfo = (CPDoubleInfo*) cpItem;
        const Variable highVar{VariableType_DOUBLE, integerInfo->highBytes};
        thread->currentFrame->operands.push_back(highVar);
        const Variable lowVar{VariableType_DOUBLE, integerInfo->lowBytes};

        thread->currentFrame->operands.push_back(lowVar);
    }
}

void ldc(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    const u1 index = args[0];
    loadConstant(thread, index, heap, VM);
}

void ldc_w(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    const u2 index = (args[0] << 8) | args[1];
    loadConstant(thread, index, heap, VM);
}

void ldc2_w(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    const u2 index = (args[0] << 8) | args[1];
    loadConstant2(thread, index);
}
