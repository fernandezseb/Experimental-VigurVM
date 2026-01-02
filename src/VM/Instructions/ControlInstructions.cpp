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

#include "ControlInstructions.h"

#include "VM/VM.h"
#include "Data/VData.h"

static void returnCat1Var(VMThread* thread)
{
    const vdata returnVal = thread->m_currentFrame->popOperand();
    thread->popFrame();
    thread->returnVar(returnVal);
}

void gotoInstruction(const InstructionInput& input)
{
    const u1 byte1 = input.args[0];
    const u1 byte2 = input.args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    input.thread->m_pc = input.thread->m_pc-3+branchByte;

    if (input.thread->m_pc < 0 || input.thread->m_pc >= input.thread->m_currentMethod->code->codeLength)
    {
        printf("Goto instruction resulted in invalid PC value: %d\n", input.thread->m_pc);
        input.thread->internalError("goto instruction created invalid PC value");
    }
}

void lookupswitch(const InstructionInput& input)
{
    const vint intValue = input.thread->m_currentFrame->popInt();
    const u4 instructionIndex = input.thread->m_pc-1;

    while ((input.thread->m_pc % 4) != 0) {
        input.thread->readUnsignedByte();
    }

    const i4 defaultOffset = input.thread->readSignedInt();
    const i4 defaultAddress = instructionIndex + defaultOffset;
    const i4 nPairs = input.thread->readSignedInt();

    for (i4 currentPair = 0; currentPair < nPairs; currentPair++) {
        const i4 matchKey = input.thread->readSignedInt();
        const i4 offset = input.thread->readSignedInt();

        // printf("%24" PRIi32, matchKey);
        //
        // printf(": %" PRIi32, (instructionIndex + offset));
        // printf("\n");

        if (intValue == matchKey)
        {
            input.thread->m_pc = instructionIndex + offset;
            return;
        }
    }
    input.thread->m_pc = defaultAddress;
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
    const vdata long1 = input.thread->m_currentFrame->popOperand();
    [[maybe_unused]] const vdata long2 = input.thread->m_currentFrame->popOperand();
    input.thread->popFrame();
    long1.checkType(VariableType_LONG);
    input.thread->returnVar(long1);
}

void dreturnInstruction(const InstructionInput& input)
{
    const vdata double1 = input.thread->m_currentFrame->popOperand();
    const vdata double2 = input.thread->m_currentFrame->popOperand();
    input.thread->popFrame();
    double1.checkType(VariableType_DOUBLE);
    input.thread->returnVar(double1);
}

void areturnInstruction(const InstructionInput& input)
{
    returnCat1Var(input.thread);
}

void returnInstruction(const InstructionInput& input)
{
    input.thread->popFrame();
}
