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


void iload(INSTRUCTION_ARGS)
{
    const u1 index = args[0];
    const Variable var = thread->m_currentFrame->localVariables[index];
    VM::checkType(var, VariableType_INT, thread);

    thread->m_currentFrame->operands.push_back(var);
}

void aload(INSTRUCTION_ARGS) {
    const u1 index = args[0];
    const Variable var = thread->m_currentFrame->localVariables[index];
    VM::checkType(var, VariableType_REFERENCE, thread);
    thread->m_currentFrame->operands.push_back(var);
}

void aload_i(INSTRUCTION_ARGS)
{
    const Variable var = thread->m_currentFrame->localVariables[arg];
    VM::checkType(var, VariableType_REFERENCE, thread);

    thread->m_currentFrame->operands.push_back(var);
}

void iload_i(INSTRUCTION_ARGS)
{
    const Variable var = thread->m_currentFrame->localVariables[arg];
    VM::checkType(var, VariableType_INT, thread);

    thread->m_currentFrame->operands.push_back(var);
}

void fload_i(INSTRUCTION_ARGS)
{
    const Variable var = thread->m_currentFrame->localVariables[arg];
    VM::checkType(var, VariableType_FLOAT, thread);

    thread->m_currentFrame->operands.push_back(var);
}

void iaload(INSTRUCTION_ARGS)
{
    const Variable index = thread->m_currentFrame->popOperand();
    const Variable arrayRef = thread->m_currentFrame->popOperand();

    const Array* array = heap->getArray(arrayRef.data);
    const i4* intArray = (i4*) array->data;

    const i4 data = intArray[index.data];

    const Variable dataVar{VariableType_INT, std::bit_cast<u4>(data)};
    thread->m_currentFrame->operands.push_back(dataVar);
}

void aaload(INSTRUCTION_ARGS)
{
    const Variable index = thread->m_currentFrame->popOperand();
    const Variable arrayRef = thread->m_currentFrame->popOperand();

    const Array* array = heap->getArray(arrayRef.data);
    const u4* referenceArray = (u4*) array->data;

    const u4 data = referenceArray[index.data];

    const Variable dataVar{VariableType_REFERENCE, std::bit_cast<u4>(data)};
    thread->m_currentFrame->operands.push_back(dataVar);
}

void caload(INSTRUCTION_ARGS)
{
    const Variable index = thread->m_currentFrame->popOperand();
    const Variable arrayRef = thread->m_currentFrame->popOperand();

    const Array* array = heap->getArray(arrayRef.data);
    const i1* intArray = (i1*) array->data;

    const i1 data = intArray[index.data];

    const Variable dataVar{VariableType_INT, static_cast<uint32_t>(static_cast<int32_t>(data))};
    thread->m_currentFrame->operands.push_back(dataVar);
}
