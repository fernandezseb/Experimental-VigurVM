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

void fcmpl(INSTRUCTION_ARGS)
{
    const Variable var2 = thread->m_currentFrame->popOperand();
    const Variable var1 = thread->m_currentFrame->popOperand();
    VM->checkType(var2, VariableType_FLOAT, thread);
    VM->checkType(var1, VariableType_FLOAT, thread);

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

    thread->m_currentFrame->operands.emplace_back(Variable{VariableType_INT, resultUnsigned});

}

void fcmpg(INSTRUCTION_ARGS)
{
    const Variable var2 = thread->m_currentFrame->popOperand();
    const Variable var1 = thread->m_currentFrame->popOperand();
    VM->checkType(var2, VariableType_FLOAT, thread);
    VM->checkType(var1, VariableType_FLOAT, thread);

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

    thread->m_currentFrame->operands.emplace_back(Variable{VariableType_INT, resultUnsigned});
}

void ifeq(INSTRUCTION_ARGS)
{
    const u1 byte1 = args[0];
    const u1 byte2 = args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    const Variable intVar = thread->m_currentFrame->popOperand();
    VM->checkType(intVar, VariableType_INT, thread);
    if (intVar.data == 0)
    {
        thread->m_pc = thread->m_pc-3+branchByte;
    }
}

void ifne(INSTRUCTION_ARGS)
{
    const u1 byte1 = args[0];
    const u1 byte2 = args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    const Variable intVar = thread->m_currentFrame->popOperand();
    VM->checkType(intVar, VariableType_INT, thread);

    if (intVar.data != 0)
    {
        thread->m_pc = thread->m_pc-3+branchByte;
    }
}

void ifge(INSTRUCTION_ARGS)
{
    const u1 byte1 = args[0];
    const u1 byte2 = args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    const Variable intVar = thread->m_currentFrame->popOperand();
    VM->checkType(intVar, VariableType_INT, thread);

    if (std::bit_cast<i4>(intVar.data) >= 0)
    {
        thread->m_pc = thread->m_pc-3+branchByte;
    }
}

void ifgt(INSTRUCTION_ARGS)
{
    const u1 byte1 = args[0];
    const u1 byte2 = args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    const Variable intVar = thread->m_currentFrame->popOperand();
    VM->checkType(intVar, VariableType_INT, thread);

    if (std::bit_cast<i4>(intVar.data) > 0)
    {
        thread->m_pc = thread->m_pc-3+branchByte;
    }
}

void ifle(INSTRUCTION_ARGS)
{
    const u1 byte1 = args[0];
    const u1 byte2 = args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    const Variable intVar = thread->m_currentFrame->popOperand();
    VM->checkType(intVar, VariableType_INT, thread);

    if (std::bit_cast<i4>(intVar.data) <= 0)
    {
        thread->m_pc = thread->m_pc-3+branchByte;
    }
}

void if_icmpne(INSTRUCTION_ARGS)
{
    const u1 byte1 = args[0];
    const u1 byte2 = args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    const Variable var2 = thread->m_currentFrame->popOperand();
    const Variable var1 = thread->m_currentFrame->popOperand();

    if (std::bit_cast<i4>(var1.data) != std::bit_cast<i4>(var2.data))
    {
        thread->m_pc = thread->m_pc-3+branchByte;
    }
}

void if_icmplt(INSTRUCTION_ARGS)
{
    const u1 byte1 = args[0];
    const u1 byte2 = args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    const Variable var2 = thread->m_currentFrame->popOperand();
    const Variable var1 = thread->m_currentFrame->popOperand();

    if (std::bit_cast<i4>(var1.data) < std::bit_cast<i4>(var2.data))
    {
        thread->m_pc = thread->m_pc-3+branchByte;
    }
}

void if_icmpgt(INSTRUCTION_ARGS)
{
    const u1 byte1 = args[0];
    const u1 byte2 = args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    Variable var2 = thread->m_currentFrame->popOperand();
    Variable var1 = thread->m_currentFrame->popOperand();

    if (std::bit_cast<i4>(var1.data) > std::bit_cast<i4>(var2.data))
    {
        thread->m_pc = thread->m_pc-3+branchByte;
    }
}

void if_icmple(INSTRUCTION_ARGS)
{
    const u1 byte1 = args[0];
    const u1 byte2 = args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    Variable var2 = thread->m_currentFrame->popOperand();
    Variable var1 = thread->m_currentFrame->popOperand();

    if (std::bit_cast<i4>(var1.data) <= std::bit_cast<i4>(var2.data))
    {
        thread->m_pc = thread->m_pc-3+branchByte;
    }
}

void if_acmpne(INSTRUCTION_ARGS)
{
    const u1 byte1 = args[0];
    const u1 byte2 = args[1];

    const i2 branchByte = (byte1 << 8) | byte2;

    Variable var2 = thread->m_currentFrame->popOperand();
    Variable var1 = thread->m_currentFrame->popOperand();

    if (std::bit_cast<i4>(var1.data) != std::bit_cast<i4>(var2.data))
    {
        thread->m_pc = thread->m_pc-3+branchByte;
    }
}
