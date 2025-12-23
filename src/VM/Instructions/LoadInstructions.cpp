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

#include "LoadInstructions.h"

#include <bit>

#include "VM/VM.h"
#include "Data/Variable.h"


void iload(const InstructionInput& input)
{
    const u1 index = input.args[0];
    const Variable var = input.thread->m_currentFrame->localVariables[index];
    var.checkType(VariableType_INT);

    input.thread->m_currentFrame->operands.push_back(var);
}

void lload(const InstructionInput& input)
{
    const u1 index = input.args[0];
    const Variable highBytes = input.thread->m_currentFrame->localVariables[index];
    highBytes.checkType( VariableType_LONG);

    const Variable lowBytes = input.thread->m_currentFrame->localVariables[index+1];
    lowBytes.checkType(VariableType_LONG);

    input.thread->m_currentFrame->operands.push_back(highBytes);
    input.thread->m_currentFrame->operands.push_back(lowBytes);
}

void fload(const InstructionInput& input)
{
    const u1 index = input.args[0];
    const Variable var = input.thread->m_currentFrame->localVariables[index];
    var.checkType(VariableType_FLOAT);

    input.thread->m_currentFrame->operands.push_back(var);
}

void aload(const InstructionInput& input) {
    const u1 index = input.args[0];
    const Variable var = input.thread->m_currentFrame->localVariables[index];
    var.checkType(VariableType_REFERENCE);
    input.thread->m_currentFrame->operands.push_back(var);
}

void aload_i(const InstructionInput& input)
{
    const Variable var = input.thread->m_currentFrame->localVariables[input.arg];
    var.checkType(VariableType_REFERENCE);

    input.thread->m_currentFrame->operands.push_back(var);
}

void iload_i(const InstructionInput& input)
{
    const Variable var = input.thread->m_currentFrame->localVariables[input.arg];
    var.checkType(VariableType_INT);

    input.thread->m_currentFrame->operands.push_back(var);
}

void lload_i(const InstructionInput& input)
{
    const Variable highBytes = input.thread->m_currentFrame->localVariables[input.arg];
    highBytes.checkType(VariableType_LONG);

    const Variable lowBytes = input.thread->m_currentFrame->localVariables[input.arg+1];
    lowBytes.checkType(VariableType_LONG);

    input.thread->m_currentFrame->operands.push_back(highBytes);
    input.thread->m_currentFrame->operands.push_back(lowBytes);
}

void fload_i(const InstructionInput& input)
{
    const Variable var = input.thread->m_currentFrame->localVariables[input.arg];
    var.checkType(VariableType_FLOAT);

    input.thread->m_currentFrame->operands.push_back(var);
}

void iaload(const InstructionInput& input)
{
    const Variable index = input.thread->m_currentFrame->popOperand();
    const Variable arrayRef = input.thread->m_currentFrame->popOperand();

    const Array* array = VM::get()->getHeap()->getArray(arrayRef.data);
    const i4* intArray = (i4*) array->data;

    const i4 data = intArray[index.data];

    const Variable dataVar{VariableType_INT, std::bit_cast<u4>(data)};
    input.thread->m_currentFrame->operands.push_back(dataVar);
}

void aaload(const InstructionInput& input)
{
    const Variable index = input.thread->m_currentFrame->popOperand();
    const Variable arrayRef = input.thread->m_currentFrame->popOperand();

    const Array* array = VM::get()->getHeap()->getArray(arrayRef.data);
    const u4* referenceArray = (u4*) array->data;

    const u4 data = referenceArray[index.data];

    const Variable dataVar{VariableType_REFERENCE, std::bit_cast<u4>(data)};
    input.thread->m_currentFrame->operands.push_back(dataVar);
}

void baload(const InstructionInput& input)
{
    const Variable index = input.thread->m_currentFrame->popOperand();
    const Variable arrayRef = input.thread->m_currentFrame->popOperand();

    const Array* array = VM::get()->getHeap()->getArray(arrayRef.data);
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

    const Array* array = VM::get()->getHeap()->getArray(arrayRef.data);
    const u2* intArray = reinterpret_cast<u2*>(array->data);

    const i4 indexSigned = static_cast<i4>(index.data);
    if ((indexSigned > array->length) || (indexSigned < 0))
    {
        input.thread->internalError("Index out of bound");
    }
    const u2 data = intArray[indexSigned];

    const Variable dataVar{VariableType_INT, std::bit_cast<u4>(static_cast<i4>(data))};
    input.thread->m_currentFrame->operands.push_back(dataVar);
}
