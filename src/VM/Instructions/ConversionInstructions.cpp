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
    const vdata valueVar = input.thread->m_currentFrame->popOperand();
    const vint intValue = valueVar.getInt();
    const vlong l = intValue;
    input.thread->m_currentFrame->operands.emplace_back(vdata{VariableType_LONG, l});
    input.thread->m_currentFrame->operands.emplace_back(vdata{VariableType_LONG, l});
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

void i2b(const InstructionInput& input)
{
    const i4 intValue = input.thread->m_currentFrame->popInt();
    const u1 b = static_cast<u1>(intValue);// TODO: Check if correct
    const i4 result = b;
    input.thread->m_currentFrame->pushInt(result);
}

void i2c(const InstructionInput& input)
{
    const i4 intValue = input.thread->m_currentFrame->popInt();
    const u2 f = static_cast<u2>(intValue);// TODO: Check if correct
    const i4 result = f;
    input.thread->m_currentFrame->pushInt(result);
}
