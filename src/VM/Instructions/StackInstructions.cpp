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

#include "StackInstructions.h"

#include "VM/VM.h"
#include "Data/Variable.h"

void pop(const InstructionInput& input)
{
    input.thread->m_currentFrame->popOperand();
}

void dup(const InstructionInput& input)
{
    const Variable top = input.thread->m_currentFrame->popOperand();
    input.thread->m_currentFrame->operands.push_back(top);
    input.thread->m_currentFrame->operands.push_back(top);
}

void dup_x1(const InstructionInput& input) {
    const Variable value1 = input.thread->m_currentFrame->popOperand();
    const Variable value2 = input.thread->m_currentFrame->popOperand();
    input.thread->m_currentFrame->operands.push_back(value1);
    input.thread->m_currentFrame->operands.push_back(value2);
    input.thread->m_currentFrame->operands.push_back(value1);
}

void dup2(const InstructionInput& input)
{
    const Variable value1 = input.thread->m_currentFrame->popOperand();
    const Variable value2 = input.thread->m_currentFrame->popOperand();
    input.thread->m_currentFrame->operands.push_back(value2);
    input.thread->m_currentFrame->operands.push_back(value1);
    input.thread->m_currentFrame->operands.push_back(value2);
    input.thread->m_currentFrame->operands.push_back(value1);
}
