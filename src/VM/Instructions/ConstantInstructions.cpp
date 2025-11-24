/*
 * Copyright (c) 2023-2025 Sebastiaan Fernandez.
 *
 * This file is part of VigurVM.
 *
 * VigurVM is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * VigurVM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with VigurVM.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "ConstantInstructions.h"

#include "VM/VM.h"
#include "Data/Variable.h"
#include "CoreUtils.h"

#include <bit>


void nop(const InstructionInput& input)
{
}

void aconst_null(const InstructionInput& input)
{
    constexpr Variable reference{VariableType_REFERENCE};
    input.thread->m_currentFrame->operands.push_back(reference);
}

void iconst_i(const InstructionInput& input)
{
    const Variable variable{VariableType_INT,
        std::bit_cast<u4>(static_cast<int32_t>(input.arg))};
    input.thread->m_currentFrame->operands.push_back(variable);
}

void lconst_i(const InstructionInput& input)
{
    constexpr Variable variableHigh{VariableType_LONG};
    const Variable variableLow{VariableType_LONG,
        std::bit_cast<u4>(static_cast<int32_t>(input.arg))};
    input.thread->m_currentFrame->operands.push_back(variableHigh);
    input.thread->m_currentFrame->operands.push_back(variableLow);
}

void fconst_i(const InstructionInput& input)
{
    const float f = input.arg;
    const Variable variable{VariableType_FLOAT, std::bit_cast<u4>(f)};
    input.thread->m_currentFrame->operands.push_back(variable);
}

void dconst_i(const InstructionInput& input)
{
    const double d = input.arg;
    const u4 lowBytes =  castToU4(*std::bit_cast<u8*>(&d));
    const u4 highBytes = (*(std::bit_cast<u8*>)(&(d)) >> 32);

    // /uint64_t bytes = ((uint64_t)highBytes << 32) + (uint64_t)lowBytes;
    // The value is double back = *reinterpret_cast<double*> (&bytes);
    const Variable variableHigh{VariableType_DOUBLE, highBytes};
    const Variable variableLow{VariableType_DOUBLE, lowBytes};
    input.thread->m_currentFrame->operands.push_back(variableHigh);
    input.thread->m_currentFrame->operands.push_back(variableLow);
}

void bipush(const InstructionInput& input)
{
    const i1 byte = std::bit_cast<u1>(input.args[0]);
    const i4 intVal = byte;
    const Variable variable{VariableType_INT, static_cast<u4>(intVal)};
    input.thread->m_currentFrame->operands.push_back(variable);
}

void sipush(const InstructionInput& input)
{
    const i2 shortValue = (input.args[0] << 8) | input.args[1];
    const i4 intValue = shortValue;
    const Variable variable{VariableType_INT, static_cast<uint32_t>(intValue)};
    input.thread->m_currentFrame->operands.push_back(variable);
}

void loadConstant(VMThread* thread, const u4 index, JavaHeap* heap, VM* VM)
{
    const ConstantPoolItem* cpItem = thread->m_currentFrame->constantPool->constants[index - 1];
    if (cpItem->getType() == CT_INTEGER)
    {
        const auto* integerInfo = static_cast<const CPIntegerInfo*>(cpItem);
        const Variable var{VariableType_INT, integerInfo->bytes};
        thread->m_currentFrame->operands.push_back(var);
    }
    else if (cpItem->getType() == CT_FLOAT)
    {
        const auto* floatInfo = static_cast<const CPFloatInfo*>(cpItem);
        const Variable var{VariableType_FLOAT, floatInfo->bytes};
        thread->m_currentFrame->operands.push_back(var);
    }
    else if (cpItem->getType() == CT_STRING)
    {
        const auto* stringInfo = static_cast<const CPStringInfo*>(cpItem);
        const std::string_view utf8String = thread->m_currentClass->constantPool->getString(stringInfo->stringIndex);
        const uint32_t strObjectId = heap->createString(utf8String.data(), VM);
        const Variable strVar{VariableType_REFERENCE, strObjectId};
        thread->m_currentFrame->operands.push_back(strVar);
    }
    else if (cpItem->getType() == CT_CLASS)
    {
        const auto* classInfo = static_cast<const CPClassInfo*>(cpItem);
        std::string_view className = thread->m_currentClass->constantPool->getString(classInfo->nameIndex);
        ClassInfo* classInfoPtr{nullptr};
        classInfoPtr = VM->getClass(className.data(), thread);
        const u4 classObjectRef =  heap->createClassObject(classInfoPtr, VM, className);
        const Variable classObjectVar{VariableType_REFERENCE, classObjectRef};
        thread->m_currentFrame->operands.push_back(classObjectVar);
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
    const ConstantPoolItem* cpItem = thread->m_currentFrame->constantPool->constants[index-1];
    if (cpItem->getType() == CT_LONG)
    {
        const CPLongInfo* integerInfo = (CPLongInfo*) cpItem;
        const Variable highVar{VariableType_LONG, integerInfo->highBytes};
        thread->m_currentFrame->operands.push_back(highVar);
        const Variable lowVar{VariableType_LONG, integerInfo->lowBytes};
        thread->m_currentFrame->operands.push_back(lowVar);
    }
    else if (cpItem->getType() == CT_DOUBLE)
    {
        const CPDoubleInfo* integerInfo = (CPDoubleInfo*) cpItem;
        const Variable highVar{VariableType_DOUBLE, integerInfo->highBytes};
        thread->m_currentFrame->operands.push_back(highVar);
        const Variable lowVar{VariableType_DOUBLE, integerInfo->lowBytes};

        thread->m_currentFrame->operands.push_back(lowVar);
    }
}

void ldc(const InstructionInput& input)
{
    const u1 index = input.args[0];
    loadConstant(input.thread, index, input.heap, input.vm);
}

void ldc_w(const InstructionInput& input)
{
    const u2 index = (input.args[0] << 8) | input.args[1];
    loadConstant(input.thread, index, input.heap, input.vm);
}

void ldc2_w(const InstructionInput& input)
{
    const u2 index = (input.args[0] << 8) | input.args[1];
    loadConstant2(input.thread, index);
}
