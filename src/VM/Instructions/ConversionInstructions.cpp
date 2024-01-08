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

#include "ConversionInstructions.h"

#include <bit>

#include "VM/VM.h"
#include "Data/Variable.h"


void i2l(INSTRUCTION_ARGS)
{
    const Variable valueVar = thread->m_currentFrame->popOperand();
    VM->checkType(valueVar, VariableType_INT, thread);
    const i4 intValue = std::bit_cast<i4>(valueVar.data);
    const long l = intValue;
    const u4* bytes = (u4*)(&l);
    const u4 highByte = bytes[1];
    const u4 lowByte = bytes[0];
    thread->m_currentFrame->operands.emplace_back(Variable{VariableType_LONG, highByte});
    thread->m_currentFrame->operands.emplace_back(Variable{VariableType_LONG, lowByte});
}

void i2f(INSTRUCTION_ARGS)
{
    const Variable valueVar = thread->m_currentFrame->popOperand();
    const i4 intValue = std::bit_cast<i4>(valueVar.data);
    const float f = static_cast<float>(intValue);
    thread->m_currentFrame->operands.emplace_back(Variable{VariableType_FLOAT, std::bit_cast<u4>(f)});
}
