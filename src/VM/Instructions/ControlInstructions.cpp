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

void gotoInstruction(INSTRUCTION_ARGS)
{
    u1 byte1 = args[0];
    u1 byte2 = args[1];

    i2 branchByte = (byte1 << 8) | byte2;

    thread->m_pc = thread->m_pc-3+branchByte;
}

void ireturnInstruction(INSTRUCTION_ARGS)
{
    StackFrame* stackFrame = thread->m_currentFrame;
    thread->m_pc = stackFrame->previousPc;
    thread->m_currentClass = stackFrame->previousClass;
    thread->m_currentMethod = stackFrame->previousMethod;

    Variable returnVal = thread->m_currentFrame->popOperand();

    thread->m_stackstack.top().frames.pop_back();
    if (thread->m_stackstack.top().frames.size() > 0)
    {
        thread->m_currentFrame = &thread->m_stackstack.top().frames[thread->m_stackstack.top().frames.size()-1];
        thread->m_currentFrame->operands.push_back(returnVal);
    } else
    {
        thread->m_currentFrame = 0;
    }
}

void areturnInstruction(INSTRUCTION_ARGS)
{
    StackFrame* stackFrame = thread->m_currentFrame;
    thread->m_pc = stackFrame->previousPc;
    thread->m_currentClass = stackFrame->previousClass;
    thread->m_currentMethod = stackFrame->previousMethod;

    Variable returnVal = thread->m_currentFrame->popOperand();

    thread->m_stackstack.top().frames.pop_back();
    if (thread->m_stackstack.top().frames.size() > 0)
    {
        thread->m_currentFrame = &thread->m_stackstack.top().frames[thread->m_stackstack.top().frames.size()-1];
        thread->m_currentFrame->operands.push_back(returnVal);
    } else
    {
        thread->m_currentFrame = 0;
    }
}

void returnInstruction(INSTRUCTION_ARGS)
{
    StackFrame* stackFrame = thread->m_currentFrame;
    thread->m_pc = stackFrame->previousPc;
    thread->m_currentClass = stackFrame->previousClass;
    thread->m_currentMethod = stackFrame->previousMethod;
    thread->m_stackstack.top().frames.pop_back();
    if (thread->m_stackstack.top().frames.size() > 0)
    {
        thread->m_currentFrame = &thread->m_stackstack.top().frames[thread->m_stackstack.top().frames.size()-1];
    } else
    {
        thread->m_currentFrame = 0;
    }
}
