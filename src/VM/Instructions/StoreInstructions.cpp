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
#include "Data/Variable.h"

void istore(const InstructionInput& input)
{
    const Variable refVar = input.thread->m_currentFrame->popOperand();
    input.vm->checkType(refVar, VariableType_INT, input.thread);
    const u1 index = input.args[0];
    input.thread->m_currentFrame->localVariables[index] = refVar;
}

void fstore(const InstructionInput& input)
{
    const Variable refVar = input.thread->m_currentFrame->popOperand();
    const u1 index = input.args[0];
    input.thread->m_currentFrame->localVariables[index] = refVar;
}

void astore(const InstructionInput& input)
{
    const Variable refVar = input.thread->m_currentFrame->popOperand();
    const u1 index = input.args[0];
    input.thread->m_currentFrame->localVariables[index] = refVar;
}

void istore_i(const InstructionInput& input)
{
    const Variable refVar = input.thread->m_currentFrame->popOperand();
    VM::checkType(refVar, VariableType_INT, input.thread);
    input.thread->m_currentFrame->localVariables[input.arg] = refVar;
}

void lstore_i(const InstructionInput& input)
{
    const i8 longVal = input.thread->m_currentFrame->popLong();
    const auto parts = reinterpret_cast<const u4*>(&longVal);
    input.thread->m_currentFrame->localVariables[input.arg] = Variable{VariableType_LONG, parts[1]};
    input.thread->m_currentFrame->localVariables[input.arg+1] = Variable{VariableType_LONG, parts[0]};
}

void astore_i(const InstructionInput& input)
{
    const Variable refVar = input.thread->m_currentFrame->popOperand();
    input.thread->m_currentFrame->localVariables[input.arg] = refVar;
}

void iastore(const InstructionInput& input)
{
    Variable value = input.thread->m_currentFrame->popOperand();
    const Variable index = input.thread->m_currentFrame->popOperand();
    const Variable arrayref = input.thread->m_currentFrame->popOperand();

    const Array* array = input.heap->getArray(arrayref.data);
    i4* intArray = reinterpret_cast<i4*>(array->data);
    intArray[index.data] = *reinterpret_cast<i4*>(&value.data);
}

void aastore(const InstructionInput& input)
{
    StackFrame* currentFrame = input.thread->m_currentFrame;
    const Variable value = currentFrame->popOperand();
    const Variable index = currentFrame->popOperand();
    const Variable arrayRef = currentFrame->popOperand();

    VM::checkType(value, VariableType_REFERENCE, input.thread);
    VM::checkType(index, VariableType_INT, input.thread);
    VM::checkType(arrayRef, VariableType_REFERENCE, input.thread);

    const Array* arrayArr = input.heap->getArray(arrayRef.data);

    if (arrayArr->arrayType != AT_REFERENCE)
    {
        input.thread->internalError("Array type must be REFERENCE");
    }

    auto* arrData = reinterpret_cast<uint32_t*>(arrayArr->data);
    arrData[index.data] = value.data;
}

void bastore(const InstructionInput& input)
{
    Variable value = input.thread->m_currentFrame->popOperand();
    const Variable index = input.thread->m_currentFrame->popOperand();
    const Variable arrayref = input.thread->m_currentFrame->popOperand();

    const Array* array = input.heap->getArray(arrayref.data);
    i4* intArray = reinterpret_cast<i4*>(array->data);
    intArray[index.data] = *reinterpret_cast<i4*>(&value.data);
}

void castore(const InstructionInput& input)
{
    // TODO: Fix for utf-16
    StackFrame* currentFrame = input.thread->m_currentFrame;
    const Variable value = currentFrame->popOperand();
    const Variable index = currentFrame->popOperand();
    const Variable arrayref = currentFrame->popOperand();

    const Array* array = input.heap->getArray(arrayref.data);

    u2* charArray = reinterpret_cast<u2*>(array->data);
    charArray[index.data] = castToU2<u4>(value.data); // XXX
}
