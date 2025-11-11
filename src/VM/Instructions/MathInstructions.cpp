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

#include "MathInstructions.h"

#include <bit>

#include "VM/VM.h"
#include "Data/Variable.h"

void iadd(const InstructionInput& input)
{
    const i4 val2 = input.thread->m_currentFrame->popInt();
    const i4 val1 = input.thread->m_currentFrame->popInt();
    input.thread->m_currentFrame->pushInt(val1+val2);
}

void ladd(const InstructionInput& input)
{
    StackFrame* currentFrame = input.thread->m_currentFrame;
    const i8 long2 = currentFrame->popLong();
    const i8 long1 = currentFrame->popLong();
    const i8 sum = long1 + long2;
    input.thread->m_currentFrame->pushLong(sum);
}

void isub(const InstructionInput& input)
{
    const i4 val2 = input.thread->m_currentFrame->popInt();
    const i4 val1 = input.thread->m_currentFrame->popInt();
    input.thread->m_currentFrame->pushInt(val1-val2);
}

void imul(const InstructionInput& input)
{
    const i4 val2 = input.thread->m_currentFrame->popInt();
    const i4 val1 = input.thread->m_currentFrame->popInt();
    input.thread->m_currentFrame->pushInt(val1*val2);
}

void fmul(const InstructionInput& input)
{
    const Variable var2 = input.thread->m_currentFrame->popOperand();
    const Variable var1 = input.thread->m_currentFrame->popOperand();
    const Variable added{VariableType_FLOAT,
        std::bit_cast<u4>(std::bit_cast<float>(var1.data)
            * std::bit_cast<float>(var2.data))};
    input.thread->m_currentFrame->operands.push_back(added);
}

void idiv(const InstructionInput& input)
{
    const i4 val2 = input.thread->m_currentFrame->popInt();
    const i4 val1 = input.thread->m_currentFrame->popInt();
    const i4 result = val1/val2;
    input.thread->m_currentFrame->pushInt(result);
}

void fdiv(const InstructionInput& input)
{
    const Variable var2 = input.thread->m_currentFrame->popOperand();
    const Variable var1 = input.thread->m_currentFrame->popOperand();
    const Variable added{VariableType_FLOAT,
        std::bit_cast<u4>(std::bit_cast<float>(var1.data)
            / std::bit_cast<float>(var2.data))};
    input.thread->m_currentFrame->operands.push_back(added);
}

void irem(const InstructionInput& input)
{
    const i4 val2 = input.thread->m_currentFrame->popInt();
    const i4 val1 = input.thread->m_currentFrame->popInt();
    const i4 result = val1%val2;
    // TODO: Fix for edge cases
    input.thread->m_currentFrame->pushInt(result);
}

void ineg(const InstructionInput& input)
{
    const i4 val = input.thread->m_currentFrame->popInt();
    input.thread->m_currentFrame->pushInt(-val);
}

void lshl(const InstructionInput& input)
{
    StackFrame* currentFrame = input.thread->m_currentFrame;
    const i4 int2 = currentFrame->popInt();
    const i8 long1 = currentFrame->popLong();
    const i8 result = long1 << (int2 & 0x3f);
    input.thread->m_currentFrame->pushLong(result);
}

void ishl(const InstructionInput& input)
{
    const Variable value2 = input.thread->m_currentFrame->popOperand();
    const Variable value1 = input.thread->m_currentFrame->popOperand();

    const i4 s = ((i4)value2.data) & 0x1f;
    const i4 resultVal = value1.data << s;
    const Variable result{VariableType_INT,
        std::bit_cast<u4>(resultVal)};

    input.thread->m_currentFrame->operands.push_back(result);
}

void ishr(const InstructionInput& input)
{
    const Variable value2 = input.thread->m_currentFrame->popOperand();
    const Variable value1 = input.thread->m_currentFrame->popOperand();

    const i4 sIntValue = static_cast<i4>(value2.data);
    const i4 valueIntValue = static_cast<i4>(value1.data);
    const i4 s = sIntValue & 0x1f;
    const i4 resultVal = valueIntValue >> s;
    const Variable result{VariableType_INT,
        std::bit_cast<u4>(resultVal)};

    input.thread->m_currentFrame->operands.push_back(result);
}

void iushr(const InstructionInput& input)
{
    Variable value2 = input.thread->m_currentFrame->popOperand();
    Variable value1 = input.thread->m_currentFrame->popOperand();

    const i4 s = ((i4)value2.data) & 0x1f;
    i4 resultVal = ((i4)value1.data >> s);

    if ((i4)value1.data < 0)
    {
        resultVal = ((i4)value1.data >> s) + (2 << ~s) ;
    }

    const Variable result{VariableType_INT,
        std::bit_cast<u4>(resultVal)};
    input.thread->m_currentFrame->operands.push_back(result);
}

void iand(const InstructionInput& input)
{
    const i4 val2 = input.thread->m_currentFrame->popInt();
    const i4 val1 = input.thread->m_currentFrame->popInt();
    input.thread->m_currentFrame->pushInt(val1&val2);
}

void land(const InstructionInput& input)
{
    StackFrame* currentFrame = input.thread->m_currentFrame;
    const i8 long2 = currentFrame->popLong();
    const i8 long1 = currentFrame->popLong();
    const i8 conjunction = long1 & long2;
    input.thread->m_currentFrame->pushLong(conjunction);
}

void ior(const InstructionInput& input)
{
    const Variable value2 = input.thread->m_currentFrame->popOperand();
    const Variable value1 = input.thread->m_currentFrame->popOperand();

    const u4 resultVal = value1.data | value2.data;

    const Variable result{VariableType_INT,
        std::bit_cast<u4>(resultVal)};
    input.thread->m_currentFrame->operands.push_back(result);
}

void ixor(const InstructionInput& input)
{
    const Variable value2 = input.thread->m_currentFrame->popOperand();
    const Variable value1 = input.thread->m_currentFrame->popOperand();

    const u4 resultVal = value1.data ^ value2.data;

    const Variable result{VariableType_INT,
        std::bit_cast<u4>(resultVal)};
    input.thread->m_currentFrame->operands.push_back(result);
}

void iinc(const InstructionInput& input)
{
    const u1 index = input.args[0];
    const i1* argsArr = ((i1*)input.args);
    const i1 constData = argsArr[1];
    Variable* var =  &input.thread->m_currentFrame->localVariables[index];
    var->data += constData;
}
