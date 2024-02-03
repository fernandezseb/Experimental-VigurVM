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

void gotoInstruction(INSTRUCTION_ARGS)
{
    u1 byte1 = args[0];
    u1 byte2 = args[1];

    i2 branchByte = (byte1 << 8) | byte2;

    thread->m_pc = thread->m_pc-3+branchByte;
}

void freturnInstruction(INSTRUCTION_ARGS)
{
    returnCat1Var(thread);
}

void ireturnInstruction(INSTRUCTION_ARGS)
{
    returnCat1Var(thread);
}

void lreturnInstruction(INSTRUCTION_ARGS)
{
    const Variable lowByte = thread->m_currentFrame->popOperand();
    const Variable highByte = thread->m_currentFrame->popOperand();
    thread->popFrame();
    thread->returnVar(highByte, lowByte);
}

void dreturnInstruction(INSTRUCTION_ARGS)
{
    const Variable lowByte = thread->m_currentFrame->popOperand();
    const Variable highByte = thread->m_currentFrame->popOperand();
    thread->popFrame();
    thread->returnVar(highByte, lowByte);
}

void areturnInstruction(INSTRUCTION_ARGS)
{
    returnCat1Var(thread);
}

void returnInstruction(INSTRUCTION_ARGS)
{
    thread->popFrame();
}
