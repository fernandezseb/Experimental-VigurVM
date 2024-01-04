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

void pop(INSTRUCTION_ARGS)
{
    thread->m_currentFrame->popOperand();
}

void dup(INSTRUCTION_ARGS)
{
    const Variable top = thread->m_currentFrame->popOperand();
    thread->m_currentFrame->operands.push_back(top);
    thread->m_currentFrame->operands.push_back(top);
}

void dup_x1(INSTRUCTION_ARGS) {
    const Variable value1 = thread->m_currentFrame->popOperand();
    const Variable value2 = thread->m_currentFrame->popOperand();
    thread->m_currentFrame->operands.push_back(value1);
    thread->m_currentFrame->operands.push_back(value2);
    thread->m_currentFrame->operands.push_back(value1);
}
