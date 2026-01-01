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

#include "StoreInstructions.h"

#include "CoreUtils.h"
#include "VM/VM.h"
#include "Data/VData.h"

void istore(const InstructionInput& input)
{
    const vdata refVar = input.thread->m_currentFrame->popOperand();
    refVar.checkType(VariableType_INT);
    const u1 index = input.args[0];
    input.thread->m_currentFrame->localVariables[index] = refVar;
}

void fstore(const InstructionInput& input)
{
    const vdata refVar = input.thread->m_currentFrame->popOperand();
    const u1 index = input.args[0];
    input.thread->m_currentFrame->localVariables[index] = refVar;
}

void astore(const InstructionInput& input)
{
    const vdata refVar = input.thread->m_currentFrame->popOperand();
    const u1 index = input.args[0];
    input.thread->m_currentFrame->localVariables[index] = refVar;
}

void istore_i(const InstructionInput& input)
{
    const vdata refVar = input.thread->m_currentFrame->popOperand();
    refVar.checkType(VariableType_INT);
    input.thread->m_currentFrame->localVariables[input.arg] = refVar;
}

void lstore_i(const InstructionInput& input)
{
    // TODO: Check duplication. Is it needed in the future?
    const vlong longVal = input.thread->m_currentFrame->popLong();
    input.thread->m_currentFrame->localVariables[input.arg] = vdata{VariableType_LONG, longVal};
    input.thread->m_currentFrame->localVariables[input.arg+1] = vdata{VariableType_LONG, longVal};
}

void astore_i(const InstructionInput& input)
{
    const vdata refVar = input.thread->m_currentFrame->popOperand();
    input.thread->m_currentFrame->localVariables[input.arg] = refVar;
}

void iastore(const InstructionInput& input)
{
    vdata value = input.thread->m_currentFrame->popOperand();
    const vdata index = input.thread->m_currentFrame->popOperand();
    const vdata arrayref = input.thread->m_currentFrame->popOperand();

    const Array* array = VM::get()->getHeap()->getArray(arrayref.getReference());
    i4* intArray = reinterpret_cast<i4*>(array->data);
    intArray[index.getInt()] = value.getInt();
}

void aastore(const InstructionInput& input)
{
    StackFrame* currentFrame = input.thread->m_currentFrame;
    const vdata value = currentFrame->popOperand();
    const vdata index = currentFrame->popOperand();
    const vdata arrayRef = currentFrame->popOperand();

    const Array* arrayArr = VM::get()->getHeap()->getArray(arrayRef.getReference());

    if (arrayArr->arrayType != AT_REFERENCE)
    {
        input.thread->internalError("Array type must be REFERENCE");
    }

    auto* arrData = reinterpret_cast<uint32_t*>(arrayArr->data);
    arrData[index.getInt()] = value.getReference();
}

void bastore(const InstructionInput& input)
{
    // TODO: Fix!!!!!
    vdata value = input.thread->m_currentFrame->popOperand();
    const vdata index = input.thread->m_currentFrame->popOperand();
    const vdata arrayref = input.thread->m_currentFrame->popOperand();

    const Array* array = VM::get()->getHeap()->getArray(arrayref.getReference());
    i1* byteArray = reinterpret_cast<i1*>(array->data);
    byteArray[index.getInt()] = static_cast<i1>(value.getInt());
}

void castore(const InstructionInput& input)
{
    // TODO: Fix for utf-16
    StackFrame* currentFrame = input.thread->m_currentFrame;
    const vdata value = currentFrame->popOperand();
    const vdata index = currentFrame->popOperand();
    const vdata arrayref = currentFrame->popOperand();

    const Array* array = VM::get()->getHeap()->getArray(arrayref.getReference());

    u2* charArray = reinterpret_cast<u2*>(array->data);
    charArray[index.getInt()] = castToU2<vint>(value.getInt()); // XXX
}
