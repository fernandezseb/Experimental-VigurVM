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

#include <bit>
#include <cmath>

#include "VM/VM.h"
#include "Data/Variable.h"

void fcmpl(const InstructionInput& input)
{
    const Variable var2 = input.thread->m_currentFrame->popOperand();
    const Variable var1 = input.thread->m_currentFrame->popOperand();
    input.VM->checkType(var2, VariableType_FLOAT, input.thread);
    input.VM->checkType(var1, VariableType_FLOAT, input.thread);

    const auto f2 = std::bit_cast<float>(var2.data);
    const auto f1 = std::bit_cast<float>(var1.data);

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

    const u4 resultUnsigned = std::bit_cast<u4>(result);

    input.thread->m_currentFrame->operands.emplace_back(Variable{VariableType_INT, resultUnsigned});

}

void fcmpg(const InstructionInput& input)
{
    const Variable var2 = input.thread->m_currentFrame->popOperand();
    const Variable var1 = input.thread->m_currentFrame->popOperand();
    input.VM->checkType(var2, VariableType_FLOAT, input.thread);
    input.VM->checkType(var1, VariableType_FLOAT, input.thread);

    const auto f2 = std::bit_cast<float>(var2.data);
    const auto f1 = std::bit_cast<float>(var1.data);

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

    const u4 resultUnsigned = std::bit_cast<u4>(result);

    input.thread->m_currentFrame->operands.emplace_back(Variable{VariableType_INT, resultUnsigned});
}

void ifeq(const InstructionInput& input)
{
    const u1 byte1 = input.args[0];
    const u1 byte2 = input.args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    const Variable intVar = input.thread->m_currentFrame->popOperand();
    input.VM->checkType(intVar, VariableType_INT, input.thread);
    if (intVar.data == 0)
    {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}

void ifne(const InstructionInput& input)
{
    const u1 byte1 = input.args[0];
    const u1 byte2 = input.args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    const Variable intVar = input.thread->m_currentFrame->popOperand();
    if (intVar.data != 0)
    {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}

void iflt(const InstructionInput& input)
{
    const u1 byte1 = input.args[0];
    const u1 byte2 = input.args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    const Variable intVar = input.thread->m_currentFrame->popOperand();
    input.VM->checkType(intVar, VariableType_INT, input.thread);

    if (std::bit_cast<i4>(intVar.data) < 0)
    {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}

void ifge(const InstructionInput& input)
{
    const u1 byte1 = input.args[0];
    const u1 byte2 = input.args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    const Variable intVar = input.thread->m_currentFrame->popOperand();
    input.VM->checkType(intVar, VariableType_INT, input.thread);

    if (std::bit_cast<i4>(intVar.data) >= 0)
    {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}

void ifgt(const InstructionInput& input)
{
    const u1 byte1 = input.args[0];
    const u1 byte2 = input.args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    const Variable intVar = input.thread->m_currentFrame->popOperand();
    input.VM->checkType(intVar, VariableType_INT, input.thread);

    if (std::bit_cast<i4>(intVar.data) > 0)
    {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}

void ifle(const InstructionInput& input)
{
    const u1 byte1 = input.args[0];
    const u1 byte2 = input.args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    const Variable intVar = input.thread->m_currentFrame->popOperand();
    input.VM->checkType(intVar, VariableType_INT, input.thread);

    if (std::bit_cast<i4>(intVar.data) <= 0)
    {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}

void if_icmpeq(const InstructionInput& input)
{
    const u1 byte1 = input.args[0];
    const u1 byte2 = input.args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    const Variable var2 = input.thread->m_currentFrame->popOperand();
    const Variable var1 = input.thread->m_currentFrame->popOperand();

    if (std::bit_cast<i4>(var1.data) == std::bit_cast<i4>(var2.data))
    {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}

void if_icmpne(const InstructionInput& input)
{
    const u1 byte1 = input.args[0];
    const u1 byte2 = input.args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    const Variable var2 = input.thread->m_currentFrame->popOperand();
    const Variable var1 = input.thread->m_currentFrame->popOperand();

    if (std::bit_cast<i4>(var1.data) != std::bit_cast<i4>(var2.data))
    {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}

void if_icmplt(const InstructionInput& input)
{
    const u1 byte1 = input.args[0];
    const u1 byte2 = input.args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    const Variable var2 = input.thread->m_currentFrame->popOperand();
    const Variable var1 = input.thread->m_currentFrame->popOperand();

    if (std::bit_cast<i4>(var1.data) < std::bit_cast<i4>(var2.data))
    {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}

void if_icmpge(const InstructionInput& input)
{
    const u1 byte1 = input.args[0];
    const u1 byte2 = input.args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    Variable var2 = input.thread->m_currentFrame->popOperand();
    Variable var1 = input.thread->m_currentFrame->popOperand();

    if (std::bit_cast<i4>(var1.data) >= std::bit_cast<i4>(var2.data))
    {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}

void if_icmpgt(const InstructionInput& input)
{
    const u1 byte1 = input.args[0];
    const u1 byte2 = input.args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    Variable var2 = input.thread->m_currentFrame->popOperand();
    Variable var1 = input.thread->m_currentFrame->popOperand();

    if (std::bit_cast<i4>(var1.data) > std::bit_cast<i4>(var2.data))
    {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}

void if_icmple(const InstructionInput& input)
{
    const u1 byte1 = input.args[0];
    const u1 byte2 = input.args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    Variable var2 = input.thread->m_currentFrame->popOperand();
    Variable var1 = input.thread->m_currentFrame->popOperand();

    if (std::bit_cast<i4>(var1.data) <= std::bit_cast<i4>(var2.data))
    {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}

void if_acmpeq(const InstructionInput& input)
{
    const u1 byte1 = input.args[0];
    const u1 byte2 = input.args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    Variable var2 = input.thread->m_currentFrame->popOperand();
    Variable var1 = input.thread->m_currentFrame->popOperand();

    if (std::bit_cast<i4>(var1.data) == std::bit_cast<i4>(var2.data))
    {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}

void if_acmpne(const InstructionInput& input)
{
    const u1 byte1 = input.args[0];
    const u1 byte2 = input.args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    Variable var2 = input.thread->m_currentFrame->popOperand();
    Variable var1 = input.thread->m_currentFrame->popOperand();

    if (std::bit_cast<i4>(var1.data) != std::bit_cast<i4>(var2.data))
    {
        input.thread->m_pc = input.thread->m_pc-3+branchByte;
    }
}
