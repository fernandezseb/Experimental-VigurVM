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

#include "ComparisonInstructions.h"

#include <bit>
#include <cmath>

#include "VM/VM.h"
#include "Data/VData.h"

void lcmp(const InstructionInput& input)
{
    i8 var2 = input.thread->m_currentFrame->popLong();
    i8 var1 = input.thread->m_currentFrame->popLong();
    i4 result = 0;
    if (var1 > var2)
    {
        result = 1;
    } else if (var1 == var2)
    {
        result = 0;
    } else
    {
        result = -1;
    }
    input.thread->m_currentFrame->operands.emplace_back(vdata{VariableType_INT, result});
}

void fcmpl(const InstructionInput& input)
{
    const vdata var2 = input.thread->m_currentFrame->popOperand();
    const vdata var1 = input.thread->m_currentFrame->popOperand();
    var2.checkType(VariableType_FLOAT);
    var1.checkType(VariableType_FLOAT);

    const auto f2 = var2.getFloat();
    const auto f1 = var1.getFloat();

    i4 result = 0;

    if (f1 > f2)
    {
        result = 1;
    } else if (f1 < f2)
    {
        result = -1;
    } else if (std::isnan(f1) || std::isnan(f2))
    {
        result = -1;
    }
    else if (f1 == f2)
    {
        result = 0;
    }

    input.thread->m_currentFrame->operands.emplace_back(vdata{VariableType_INT, result});

}

void fcmpg(const InstructionInput& input)
{
    const vdata var2 = input.thread->m_currentFrame->popOperand();
    const vdata var1 = input.thread->m_currentFrame->popOperand();

    const auto f2 = var2.getFloat();
    const auto f1 = var1.getFloat();

    i4 result = 0;

    if (f1 > f2)
    {
        result = 1;
    } else if (f1 < f2)
    {
        result = -1;
    } else if (std::isnan(f1) || std::isnan(f2))
    {
        result = 1;
    } else if (f1 == f2)
    {
        result = 0;
    }

    input.thread->m_currentFrame->operands.emplace_back(vdata{VariableType_INT, result});
}

void ifeq(const InstructionInput& input)
{
    const u1 byte1 = input.args[0];
    const u1 byte2 = input.args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    const vdata intVar = input.thread->m_currentFrame->popOperand();
    if (intVar.getInt() == 0)
    {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}

void ifne(const InstructionInput& input)
{
    const u1 byte1 = input.args[0];
    const u1 byte2 = input.args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    const vdata intVar = input.thread->m_currentFrame->popOperand();
    if (intVar.getInt() != 0)
    {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}

void iflt(const InstructionInput& input)
{
    const u1 byte1 = input.args[0];
    const u1 byte2 = input.args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    const vdata intVar = input.thread->m_currentFrame->popOperand();

    if (intVar.getInt() < 0)
    {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}

void ifge(const InstructionInput& input)
{
    const u1 byte1 = input.args[0];
    const u1 byte2 = input.args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    const vdata intVar = input.thread->m_currentFrame->popOperand();

    if (intVar.getInt() >= 0)
    {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}

void ifgt(const InstructionInput& input)
{
    const u1 byte1 = input.args[0];
    const u1 byte2 = input.args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    const vdata intVar = input.thread->m_currentFrame->popOperand();

    if (intVar.getInt() > 0)
    {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}

void ifle(const InstructionInput& input)
{
    const u1 byte1 = input.args[0];
    const u1 byte2 = input.args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    const vdata intVar = input.thread->m_currentFrame->popOperand();

    if (intVar.getInt() <= 0)
    {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}

void if_icmpeq(const InstructionInput& input)
{
    const u1 byte1 = input.args[0];
    const u1 byte2 = input.args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    const vdata var2 = input.thread->m_currentFrame->popOperand();
    const vdata var1 = input.thread->m_currentFrame->popOperand();

    if (var1.getInt() == var2.getInt())
    {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}

void if_icmpne(const InstructionInput& input)
{
    const u1 byte1 = input.args[0];
    const u1 byte2 = input.args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    const vdata var2 = input.thread->m_currentFrame->popOperand();
    const vdata var1 = input.thread->m_currentFrame->popOperand();

    if (var1.getInt() != var2.getInt())
    {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}

void if_icmplt(const InstructionInput& input)
{
    const u1 byte1 = input.args[0];
    const u1 byte2 = input.args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    const vdata var2 = input.thread->m_currentFrame->popOperand();
    const vdata var1 = input.thread->m_currentFrame->popOperand();

    if (var1.getInt() < var2.getInt())
    {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}

void if_icmpge(const InstructionInput& input)
{
    const u1 byte1 = input.args[0];
    const u1 byte2 = input.args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    vdata var2 = input.thread->m_currentFrame->popOperand();
    vdata var1 = input.thread->m_currentFrame->popOperand();

    if (var1.getInt() >= var2.getInt())
    {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}

void if_icmpgt(const InstructionInput& input)
{
    const u1 byte1 = input.args[0];
    const u1 byte2 = input.args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    vdata var2 = input.thread->m_currentFrame->popOperand();
    vdata var1 = input.thread->m_currentFrame->popOperand();

    if (var1.getInt() > var2.getInt())
    {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}

void if_icmple(const InstructionInput& input)
{
    const u1 byte1 = input.args[0];
    const u1 byte2 = input.args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    vdata var2 = input.thread->m_currentFrame->popOperand();
    vdata var1 = input.thread->m_currentFrame->popOperand();

    if (var1.getInt() <= var2.getInt())
    {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}

void if_acmpeq(const InstructionInput& input)
{
    const u1 byte1 = input.args[0];
    const u1 byte2 = input.args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    vdata var2 = input.thread->m_currentFrame->popOperand();
    vdata var1 = input.thread->m_currentFrame->popOperand();

    if (var1.getReference() == var2.getReference())
    {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}

void if_acmpne(const InstructionInput& input)
{
    const u1 byte1 = input.args[0];
    const u1 byte2 = input.args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    vdata var2 = input.thread->m_currentFrame->popOperand();
    vdata var1 = input.thread->m_currentFrame->popOperand();

    if (var1.getReference() != var2.getReference())
    {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}
