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

#include "StoreInstructions.h"

#include "Util.h"
#include "VM/VM.h"
#include "Data/Variable.h"

void istore(INSTRUCTION_ARGS)
{
    const Variable refVar = thread->m_currentFrame->popOperand();
    const u1 index = args[0];
    thread->m_currentFrame->localVariables[index] = refVar;
}

void astore(INSTRUCTION_ARGS)
{
    const Variable refVar = thread->m_currentFrame->popOperand();
    const u1 index = args[0];
    thread->m_currentFrame->localVariables[index] = refVar;
}

void istore_i(INSTRUCTION_ARGS)
{
    const Variable refVar = thread->m_currentFrame->popOperand();
    VM::checkType(refVar, VariableType_INT, thread);
    thread->m_currentFrame->localVariables[arg] = refVar;
}

void astore_i(INSTRUCTION_ARGS)
{
    const Variable refVar = thread->m_currentFrame->popOperand();
    thread->m_currentFrame->localVariables[arg] = refVar;
}

void iastore(INSTRUCTION_ARGS)
{
    Variable value = thread->m_currentFrame->popOperand();
    const Variable index = thread->m_currentFrame->popOperand();
    const Variable arrayref = thread->m_currentFrame->popOperand();

    const Array* array = heap->getArray(arrayref.data);
    i4* intArray = reinterpret_cast<i4*>(array->data);
    intArray[index.data] = *reinterpret_cast<i4*>(&value.data);
}

void aastore(INSTRUCTION_ARGS)
{
    StackFrame* currentFrame = thread->m_currentFrame;
    const Variable value = currentFrame->popOperand();
    const Variable index = currentFrame->popOperand();
    const Variable arrayRef = currentFrame->popOperand();

    VM::checkType(value, VariableType_REFERENCE, thread);
    VM::checkType(index, VariableType_INT, thread);
    VM::checkType(arrayRef, VariableType_REFERENCE, thread);

    const Array* arrayArr = heap->getArray(arrayRef.data);

    if (arrayArr->arrayType != AT_REFERENCE)
    {
        thread->internalError("Array type must be REFERENCE");
    }

    auto* arrData = reinterpret_cast<uint32_t*>(arrayArr->data);
    arrData[index.data] = value.data;
}

void castore(INSTRUCTION_ARGS)
{
    StackFrame* currentFrame = thread->m_currentFrame;
    const Variable value = currentFrame->popOperand();
    const Variable index = currentFrame->popOperand();
    const Variable arrayref = currentFrame->popOperand();

    const Array* array = heap->getArray(arrayref.data);
    array->data[index.data] = castToU1<u4>(value.data);
}
