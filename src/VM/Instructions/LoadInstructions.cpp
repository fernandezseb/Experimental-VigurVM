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
    const vdata var = input.thread->m_currentFrame->localVariables[index];
    var.checkType(VariableType_INT);

    input.thread->m_currentFrame->operands.push_back(var);
}

void lload(const InstructionInput& input)
{
    const u1 index = input.args[0];
    const vdata long1 = input.thread->m_currentFrame->localVariables[index];
    long1.checkType( VariableType_LONG);

    const vdata long2 = input.thread->m_currentFrame->localVariables[index+1];
    long2.checkType(VariableType_LONG);

    input.thread->m_currentFrame->operands.push_back(long1);
    input.thread->m_currentFrame->operands.push_back(long2);
}

void fload(const InstructionInput& input)
{
    const u1 index = input.args[0];
    const vdata var = input.thread->m_currentFrame->localVariables[index];
    var.checkType(VariableType_FLOAT);

    input.thread->m_currentFrame->operands.push_back(var);
}

void aload(const InstructionInput& input) {
    const u1 index = input.args[0];
    const vdata var = input.thread->m_currentFrame->localVariables[index];
    var.checkType(VariableType_REFERENCE);
    input.thread->m_currentFrame->operands.push_back(var);
}

void aload_i(const InstructionInput& input)
{
    const vdata var = input.thread->m_currentFrame->localVariables[input.arg];
    var.checkType(VariableType_REFERENCE);

    input.thread->m_currentFrame->operands.push_back(var);
}

void iload_i(const InstructionInput& input)
{
    const vdata var = input.thread->m_currentFrame->localVariables[input.arg];
    var.checkType(VariableType_INT);

    input.thread->m_currentFrame->operands.push_back(var);
}

void lload_i(const InstructionInput& input)
{
    const vdata long1 = input.thread->m_currentFrame->localVariables[input.arg];
    long1.checkType(VariableType_LONG);

    const vdata long2 = input.thread->m_currentFrame->localVariables[input.arg+1];
    long2.checkType(VariableType_LONG);

    input.thread->m_currentFrame->operands.push_back(long1);
    input.thread->m_currentFrame->operands.push_back(long2);
}

void fload_i(const InstructionInput& input)
{
    const vdata var = input.thread->m_currentFrame->localVariables[input.arg];
    var.checkType(VariableType_FLOAT);

    input.thread->m_currentFrame->operands.push_back(var);
}

void iaload(const InstructionInput& input)
{
    const vdata index = input.thread->m_currentFrame->popOperand();
    const vdata arrayRef = input.thread->m_currentFrame->popOperand();

    const Array* array = VM::get()->getHeap()->getArray(arrayRef.getReference());
    const i4* intArray = (i4*) array->data;

    const i4 data = intArray[index.getInt()];

    const vdata dataVar{VariableType_INT, static_cast<vint>(data)};
    input.thread->m_currentFrame->operands.push_back(dataVar);
}

void aaload(const InstructionInput& input)
{
    const vdata index = input.thread->m_currentFrame->popOperand();
    const vdata arrayRef = input.thread->m_currentFrame->popOperand();

    const Array* array = VM::get()->getHeap()->getArray(arrayRef.getReference());
    const u4* referenceArray = (u4*) array->data;

    const u4 data = referenceArray[index.getInt()];

    const vdata dataVar{VariableType_REFERENCE, static_cast<vreference>(data)};
    input.thread->m_currentFrame->operands.push_back(dataVar);
}

void baload(const InstructionInput& input)
{
    const vdata index = input.thread->m_currentFrame->popOperand();
    const vdata arrayRef = input.thread->m_currentFrame->popOperand();

    const Array* array = VM::get()->getHeap()->getArray(arrayRef.getReference());
    const i4* intArray = (i4*) array->data;

    const i4 data = intArray[index.getInt()];

    const vdata dataVar{VariableType_INT, static_cast<vint>(data)};
    input.thread->m_currentFrame->operands.push_back(dataVar);
}

void caload(const InstructionInput& input)
{
    // TODO: Fix for utf-16
    const vdata index = input.thread->m_currentFrame->popOperand();
    const vdata arrayRef = input.thread->m_currentFrame->popOperand();

    const Array* array = VM::get()->getHeap()->getArray(arrayRef.getReference());
    const u2* intArray = reinterpret_cast<u2*>(array->data);

    const vint indexSigned = index.getInt();
    if ((indexSigned > array->length) || (indexSigned < 0))
    {
        input.thread->internalError("Index out of bound");
    }
    const u2 data = intArray[indexSigned];

    const vdata dataVar{VariableType_INT, static_cast<vint>(data)};
    input.thread->m_currentFrame->operands.push_back(dataVar);
}
