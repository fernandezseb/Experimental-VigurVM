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

#include "ComparisonInstructions.h"

#include "VM/VM.h"
#include "Data/Variable.h"

void ifeq(INSTRUCTION_ARGS)
{
    u1 byte1 = args[0];
    u1 byte2 = args[1];

    i2 branchByte = (byte1 << 8) | byte2;

    Variable intVar = thread->m_currentFrame->popOperand();
    VM->checkType(intVar, VariableType_INT, thread);
    if (((i4)intVar.data) == 0)
    {
        thread->m_pc = thread->m_pc-3+branchByte;
    }
}

void ifne(INSTRUCTION_ARGS)
{
    u1 byte1 = args[0];
    u1 byte2 = args[1];

    i2 branchByte = (byte1 << 8) | byte2;

    Variable intVar = thread->m_currentFrame->popOperand();
    VM->checkType(intVar, VariableType_INT, thread);

    if (((i4)intVar.data) != 0)
    {
        thread->m_pc = thread->m_pc-3+branchByte;
    }
}

void ifge(INSTRUCTION_ARGS)
{
    u1 byte1 = args[0];
    u1 byte2 = args[1];

    i2 branchByte = (byte1 << 8) | byte2;

    Variable intVar = thread->m_currentFrame->popOperand();
    VM->checkType(intVar, VariableType_INT, thread);

    if (((i4)intVar.data) >= 0)
    {
        thread->m_pc = thread->m_pc-3+branchByte;
    }
}

void ifgt(INSTRUCTION_ARGS)
{
    u1 byte1 = args[0];
    u1 byte2 = args[1];

    i2 branchByte = (byte1 << 8) | byte2;

    Variable intVar = thread->m_currentFrame->popOperand();
    VM->checkType(intVar, VariableType_INT, thread);

    if (((i4)intVar.data) > 0)
    {
        thread->m_pc = thread->m_pc-3+branchByte;
    }
}

void ifle(INSTRUCTION_ARGS)
{
    u1 byte1 = args[0];
    u1 byte2 = args[1];

    i2 branchByte = (byte1 << 8) | byte2;

    Variable intVar = thread->m_currentFrame->popOperand();
    VM->checkType(intVar, VariableType_INT, thread);

    if (((i4)intVar.data) <= 0)
    {
        thread->m_pc = thread->m_pc-3+branchByte;
    }
}

void if_icmpne(INSTRUCTION_ARGS)
{
    u1 byte1 = args[0];
    u1 byte2 = args[1];

    i2 branchByte = (byte1 << 8) | byte2;

    Variable var2 = thread->m_currentFrame->popOperand();
    Variable var1 = thread->m_currentFrame->popOperand();

    if (((i4)var1.data) != ((i4)var2.data))
    {
        thread->m_pc = thread->m_pc-3+branchByte;
    }
}

void if_icmplt(INSTRUCTION_ARGS)
{
    u1 byte1 = args[0];
    u1 byte2 = args[1];

    i2 branchByte = (byte1 << 8) | byte2;

    Variable var2 = thread->m_currentFrame->popOperand();
    Variable var1 = thread->m_currentFrame->popOperand();

    if (((i4)var1.data) < ((i4)var2.data))
    {
        thread->m_pc = thread->m_pc-3+branchByte;
    }
}

void if_icmpgt(INSTRUCTION_ARGS)
{
    u1 byte1 = args[0];
    u1 byte2 = args[1];

    i2 branchByte = (byte1 << 8) | byte2;

    Variable var2 = thread->m_currentFrame->popOperand();
    Variable var1 = thread->m_currentFrame->popOperand();

    if (((i4)var1.data) > ((i4)var2.data))
    {
        thread->m_pc = thread->m_pc-3+branchByte;
    }
}

void if_icmple(INSTRUCTION_ARGS)
{
    u1 byte1 = args[0];
    u1 byte2 = args[1];

    i2 branchByte = (byte1 << 8) | byte2;

    Variable var2 = thread->m_currentFrame->popOperand();
    Variable var1 = thread->m_currentFrame->popOperand();

    if (((i4)var1.data) <= ((i4)var2.data))
    {
        thread->m_pc = thread->m_pc-3+branchByte;
    }
}

void if_acmpne(INSTRUCTION_ARGS)
{
    u1 byte1 = args[0];
    u1 byte2 = args[1];

    i2 branchByte = (byte1 << 8) | byte2;

    Variable var2 = thread->m_currentFrame->popOperand();
    Variable var1 = thread->m_currentFrame->popOperand();

    if (((i4)var1.data) != ((i4)var2.data))
    {
        thread->m_pc = thread->m_pc-3+branchByte;
    }
}
