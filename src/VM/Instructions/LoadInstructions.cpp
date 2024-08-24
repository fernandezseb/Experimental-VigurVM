/*
 * Copyright (c) 2023-2024 Sebastiaan Fernandez.
 *
 * This file is part of VigurVM.
 *
 * VigurVM is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * VigurVM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with Foobar.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "LoadInstructions.h"

#include <bit>

#include "VM/VM.h"
#include "Data/Variable.h"


void iload(const InstructionInput& input)
{
    const u1 index = input.args[0];
    const Variable var = input.thread->m_currentFrame->localVariables[index];
    VM::checkType(var, VariableType_INT, input.thread);

    input.thread->m_currentFrame->operands.push_back(var);
}

void lload(const InstructionInput& input)
{
    const u1 index = input.args[0];
    const Variable highBytes = input.thread->m_currentFrame->localVariables[index];
    VM::checkType(highBytes, VariableType_LONG, input.thread);

    const Variable lowBytes = input.thread->m_currentFrame->localVariables[index+1];
    VM::checkType(lowBytes, VariableType_LONG, input.thread);

    input.thread->m_currentFrame->operands.push_back(highBytes);
    input.thread->m_currentFrame->operands.push_back(lowBytes);
}

void fload(const InstructionInput& input)
{
    const u1 index = input.args[0];
    const Variable var = input.thread->m_currentFrame->localVariables[index];
    VM::checkType(var, VariableType_FLOAT, input.thread);

    input.thread->m_currentFrame->operands.push_back(var);
}

void aload(const InstructionInput& input) {
    const u1 index = input.args[0];
    const Variable var = input.thread->m_currentFrame->localVariables[index];
    VM::checkType(var, VariableType_REFERENCE, input.thread);
    input.thread->m_currentFrame->operands.push_back(var);
}

void aload_i(const InstructionInput& input)
{
    const Variable var = input.thread->m_currentFrame->localVariables[input.arg];
    VM::checkType(var, VariableType_REFERENCE, input.thread);

    input.thread->m_currentFrame->operands.push_back(var);
}

void iload_i(const InstructionInput& input)
{
    const Variable var = input.thread->m_currentFrame->localVariables[input.arg];
    VM::checkType(var, VariableType_INT, input.thread);

    input.thread->m_currentFrame->operands.push_back(var);
}

void lload_i(const InstructionInput& input)
{
    const Variable highBytes = input.thread->m_currentFrame->localVariables[input.arg];
    VM::checkType(highBytes, VariableType_LONG, input.thread);

    const Variable lowBytes = input.thread->m_currentFrame->localVariables[input.arg+1];
    VM::checkType(lowBytes, VariableType_LONG, input.thread);

    input.thread->m_currentFrame->operands.push_back(highBytes);
    input.thread->m_currentFrame->operands.push_back(lowBytes);
}

void fload_i(const InstructionInput& input)
{
    const Variable var = input.thread->m_currentFrame->localVariables[input.arg];
    VM::checkType(var, VariableType_FLOAT, input.thread);

    input.thread->m_currentFrame->operands.push_back(var);
}

void iaload(const InstructionInput& input)
{
    const Variable index = input.thread->m_currentFrame->popOperand();
    const Variable arrayRef = input.thread->m_currentFrame->popOperand();

    const Array* array = input.heap->getArray(arrayRef.data);
    const i4* intArray = (i4*) array->data;

    const i4 data = intArray[index.data];

    const Variable dataVar{VariableType_INT, std::bit_cast<u4>(data)};
    input.thread->m_currentFrame->operands.push_back(dataVar);
}

void aaload(const InstructionInput& input)
{
    const Variable index = input.thread->m_currentFrame->popOperand();
    const Variable arrayRef = input.thread->m_currentFrame->popOperand();

    const Array* array = input.heap->getArray(arrayRef.data);
    const u4* referenceArray = (u4*) array->data;

    const u4 data = referenceArray[index.data];

    const Variable dataVar{VariableType_REFERENCE, std::bit_cast<u4>(data)};
    input.thread->m_currentFrame->operands.push_back(dataVar);
}

void baload(const InstructionInput& input)
{
    const Variable index = input.thread->m_currentFrame->popOperand();
    const Variable arrayRef = input.thread->m_currentFrame->popOperand();

    const Array* array = input.heap->getArray(arrayRef.data);
    const i4* intArray = (i4*) array->data;

    const i4 data = intArray[index.data];

    const Variable dataVar{VariableType_INT, std::bit_cast<u4>(data)};
    input.thread->m_currentFrame->operands.push_back(dataVar);
}

void caload(const InstructionInput& input)
{
    // TODO: Fix for utf-16
    const Variable index = input.thread->m_currentFrame->popOperand();
    const Variable arrayRef = input.thread->m_currentFrame->popOperand();

    const Array* array = input.heap->getArray(arrayRef.data);
    const i1* intArray = (i1*) array->data;

    const i1 data = intArray[index.data];

    const Variable dataVar{VariableType_INT, static_cast<uint32_t>(static_cast<int32_t>(data))};
    input.thread->m_currentFrame->operands.push_back(dataVar);
}
