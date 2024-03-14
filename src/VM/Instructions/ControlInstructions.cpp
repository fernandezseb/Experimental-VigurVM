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

#include "ControlInstructions.h"

#include "VM/VM.h"
#include "Data/Variable.h"

static void returnCat1Var(VMThread* thread)
{
    const Variable returnVal = thread->m_currentFrame->popOperand();
    thread->popFrame();
    thread->returnVar(returnVal);
}

void gotoInstruction(const InstructionInput& input)
{
    u1 byte1 = input.args[0];
    u1 byte2 = input.args[1];

    i2 branchByte = (byte1 << 8) | byte2;

    input.thread->m_pc = input.thread->m_pc-3+branchByte;
}

void freturnInstruction(const InstructionInput& input)
{
    returnCat1Var(input.thread);
}

void ireturnInstruction(const InstructionInput& input)
{
    returnCat1Var(input.thread);
}

void lreturnInstruction(const InstructionInput& input)
{
    const Variable lowByte = input.thread->m_currentFrame->popOperand();
    const Variable highByte = input.thread->m_currentFrame->popOperand();
    input.thread->popFrame();
    input.thread->returnVar(highByte, lowByte);
}

void dreturnInstruction(const InstructionInput& input)
{
    const Variable lowByte = input.thread->m_currentFrame->popOperand();
    const Variable highByte = input.thread->m_currentFrame->popOperand();
    input.thread->popFrame();
    input.thread->returnVar(highByte, lowByte);
}

void areturnInstruction(const InstructionInput& input)
{
    returnCat1Var(input.thread);
}

void returnInstruction(const InstructionInput& input)
{
    input.thread->popFrame();
}
