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

#include "ConversionInstructions.h"

#include <bit>

#include "VM/VM.h"
#include "Data/Variable.h"


void i2l(const InstructionInput& input)
{
    const Variable valueVar = input.thread->m_currentFrame->popOperand();
    input.vm->checkType(valueVar, VariableType_INT, input.thread);
    const i4 intValue = std::bit_cast<i4>(valueVar.data);
    const i8 l = intValue;
    const u4* bytes = (u4*)(&l);
    const u4 highByte = bytes[1];
    const u4 lowByte = bytes[0];
    input.thread->m_currentFrame->operands.emplace_back(Variable{VariableType_LONG, highByte});
    input.thread->m_currentFrame->operands.emplace_back(Variable{VariableType_LONG, lowByte});
}

void i2f(const InstructionInput& input)
{
    const i4 intValue = input.thread->m_currentFrame->popInt();
    const float f = static_cast<float>(intValue);
    input.thread->m_currentFrame->pushFloat(f);
}

void f2i(const InstructionInput& input)
{
    const float f = input.thread->m_currentFrame->popFloat();
    const i4 intValue = static_cast<i4>(f);
    input.thread->m_currentFrame->pushInt(intValue);
}

void i2c(const InstructionInput& input)
{
    const i4 intValue = input.thread->m_currentFrame->popInt();
    const u2 f = static_cast<u2>(intValue);// TODO: Check if correct
    const i4 result = f;
    input.thread->m_currentFrame->pushInt(result);
}
