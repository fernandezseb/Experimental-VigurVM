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
#include "Data/VData.h"
#include "CoreUtils.h"

#include <bit>


void nop(const InstructionInput& input)
{
}

void aconst_null(const InstructionInput& input)
{
    constexpr vdata reference{VariableType_REFERENCE};
    input.thread->m_currentFrame->operands.push_back(reference);
}

void iconst_i(const InstructionInput& input)
{
    const vdata variable{VariableType_INT,
        static_cast<int32_t>(input.arg)};
    input.thread->m_currentFrame->operands.push_back(variable);
}

void lconst_i(const InstructionInput& input)
{
    const vdata variable(VariableType_LONG,
        static_cast<vlong>(input.arg));
    input.thread->m_currentFrame->operands.push_back(variable);
    input.thread->m_currentFrame->operands.push_back(variable);
}

void fconst_i(const InstructionInput& input)
{
    const auto f = static_cast<vfloat>(input.arg);
    const vdata variable(VariableType_FLOAT, f);
    input.thread->m_currentFrame->operands.push_back(variable);
}

void dconst_i(const InstructionInput& input)
{
    const vdouble d = static_cast<vdouble>(input.arg);

    // /uint64_t bytes = ((uint64_t)highBytes << 32) + (uint64_t)lowBytes;
    // The value is double back = *reinterpret_cast<double*> (&bytes);
    const vdata variable(VariableType_DOUBLE, d);
    input.thread->m_currentFrame->operands.push_back(variable);
    input.thread->m_currentFrame->operands.push_back(variable);
}

void bipush(const InstructionInput& input)
{
    const i1 byte = std::bit_cast<u1>(input.args[0]);
    const vint intVal = byte;
    const vdata variable{VariableType_INT, intVal};
    input.thread->m_currentFrame->operands.push_back(variable);
}

void sipush(const InstructionInput& input)
{
    const i2 shortValue = (input.args[0] << 8) | input.args[1];
    const vint intValue = static_cast<vint>(shortValue);
    const vdata variable{VariableType_INT, intValue};
    input.thread->m_currentFrame->operands.push_back(variable);
}

void loadConstant(VMThread* thread, const u4 index)
{
    const ConstantPoolItem* cpItem = thread->m_currentFrame->constantPool->constants[index - 1];
    if (cpItem->getType() == CT_INTEGER)
    {
        const auto* integerInfo = static_cast<const CPIntegerInfo*>(cpItem);
        const vdata var{VariableType_INT, static_cast<vint>(integerInfo->bytes)};
        thread->m_currentFrame->operands.push_back(var);
    }
    else if (cpItem->getType() == CT_FLOAT)
    {
        const auto* floatInfo = static_cast<const CPFloatInfo*>(cpItem);
        const vdata var{VariableType_FLOAT, std::bit_cast<vfloat>(floatInfo->bytes)};
        thread->m_currentFrame->operands.push_back(var);
    }
    else if (cpItem->getType() == CT_STRING)
    {
        const auto* stringInfo = static_cast<const CPStringInfo*>(cpItem);
        const std::string_view utf8String = thread->m_currentClass->constantPool->getString(stringInfo->stringIndex);
        const vreference strObjectId = VM::get()->getHeap()->createString(utf8String.data());
        const vdata strVar{VariableType_REFERENCE, strObjectId};
        thread->m_currentFrame->operands.push_back(strVar);
    }
    else if (cpItem->getType() == CT_CLASS)
    {
        const auto* classInfo = static_cast<const CPClassInfo*>(cpItem);
        std::string_view className = thread->m_currentClass->constantPool->getString(classInfo->nameIndex);
        ClassInfo* classInfoPtr{nullptr};
        classInfoPtr = thread->getClass(className.data());
        const vreference classObjectRef =  VM::get()->getHeap()->createClassObject(classInfoPtr, className);
        const vdata classObjectVar{VariableType_REFERENCE, classObjectRef};
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
        // XXX: XXX
        const CPLongInfo* integerInfo = (CPLongInfo*) cpItem;// TODO: Construct the long from the two parts
        const uint64_t bytes = ((uint64_t)integerInfo->highBytes << 32) + (uint64_t)integerInfo->lowBytes;
        const vlong longValue = std::bit_cast<vlong>(bytes);
        thread->m_currentFrame->operands.emplace_back(VariableType_LONG, longValue);
        thread->m_currentFrame->operands.emplace_back(VariableType_LONG, longValue);
    }
    else if (cpItem->getType() == CT_DOUBLE)
    {
        // XXX: XXX This is nonsense
        const CPDoubleInfo* integerInfo = (CPDoubleInfo*) cpItem;
        const uint64_t bytes = ((uint64_t)integerInfo->highBytes << 32) + (uint64_t)integerInfo->lowBytes;
        const vdouble doubleValue = std::bit_cast<vdouble>(bytes);
        thread->m_currentFrame->operands.emplace_back(VariableType_DOUBLE, doubleValue);
        thread->m_currentFrame->operands.emplace_back(VariableType_DOUBLE, doubleValue);
    }
}

void ldc(const InstructionInput& input)
{
    const u1 index = input.args[0];
    loadConstant(input.thread, index);
}

void ldc_w(const InstructionInput& input)
{
    const u2 index = (input.args[0] << 8) | input.args[1];
    loadConstant(input.thread, index);
}

void ldc2_w(const InstructionInput& input)
{
    const u2 index = (input.args[0] << 8) | input.args[1];
    loadConstant2(input.thread, index);
}
