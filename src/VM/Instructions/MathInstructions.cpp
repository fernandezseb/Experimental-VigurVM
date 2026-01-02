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
#include "Data/VData.h"

void iadd(const InstructionInput& input)
{
    const vint val2 = input.thread->m_currentFrame->popInt();
    const vint val1 = input.thread->m_currentFrame->popInt();
    input.thread->m_currentFrame->pushInt(val1+val2);
}

void ladd(const InstructionInput& input)
{
    StackFrame* currentFrame = input.thread->m_currentFrame;
    const vlong long2 = currentFrame->popLong();
    const vlong long1 = currentFrame->popLong();
    const vlong sum = long1 + long2;
    input.thread->m_currentFrame->pushLong(sum);
}

void isub(const InstructionInput& input)
{
    const vint val2 = input.thread->m_currentFrame->popInt();
    const vint val1 = input.thread->m_currentFrame->popInt();
    input.thread->m_currentFrame->pushInt(val1-val2);
}

void imul(const InstructionInput& input)
{
    const vint val2 = input.thread->m_currentFrame->popInt();
    const vint val1 = input.thread->m_currentFrame->popInt();
    input.thread->m_currentFrame->pushInt(val1*val2);
}

void fmul(const InstructionInput& input)
{
    const vdata var2 = input.thread->m_currentFrame->popOperand();
    const vdata var1 = input.thread->m_currentFrame->popOperand();
    const vdata added{VariableType_FLOAT,
        var2.getFloat()
            * var1.getFloat()};
    input.thread->m_currentFrame->operands.push_back(added);
}

void idiv(const InstructionInput& input)
{
    const vint val2 = input.thread->m_currentFrame->popInt();
    const vint val1 = input.thread->m_currentFrame->popInt();
    const vint result = val1/val2;
    input.thread->m_currentFrame->pushInt(result);
}

void fdiv(const InstructionInput& input)
{
    const vdata var2 = input.thread->m_currentFrame->popOperand();
    const vdata var1 = input.thread->m_currentFrame->popOperand();
    const vdata added{VariableType_FLOAT, var1.getFloat()/ var2.getFloat()};
    input.thread->m_currentFrame->operands.push_back(added);
}

void irem(const InstructionInput& input)
{
    const vint val2 = input.thread->m_currentFrame->popInt();
    const vint val1 = input.thread->m_currentFrame->popInt();
    const vint result = val1%val2;
    // TODO: Fix for edge cases
    input.thread->m_currentFrame->pushInt(result);
}

void ineg(const InstructionInput& input)
{
    const vint val = input.thread->m_currentFrame->popInt();
    input.thread->m_currentFrame->pushInt(-val);
}

void lshl(const InstructionInput& input)
{
    StackFrame* currentFrame = input.thread->m_currentFrame;
    const vint int2 = currentFrame->popInt();
    const vlong long1 = currentFrame->popLong();
    const vlong result = long1 << (int2 & 0x3f);
    input.thread->m_currentFrame->pushLong(result);
}

void ishl(const InstructionInput& input)
{
    const vdata value2 = input.thread->m_currentFrame->popOperand();
    const vdata value1 = input.thread->m_currentFrame->popOperand();

    const vint sIntValue = value2.getInt();
    const vint valueIntValue = value1.getInt();
    const vint s = sIntValue & 0x1f;
    const vint resultVal = valueIntValue << s;
    const vdata result{VariableType_INT, resultVal};

    input.thread->m_currentFrame->operands.push_back(result);}

void ishr(const InstructionInput& input)
{
    const vdata value2 = input.thread->m_currentFrame->popOperand();
    const vdata value1 = input.thread->m_currentFrame->popOperand();

    const vint sIntValue = value2.getInt();
    const vint valueIntValue = value1.getInt();
    const vint s = sIntValue & 0x1f;
    const vint resultVal = valueIntValue >> s;
    const vdata result{VariableType_INT, resultVal};

    input.thread->m_currentFrame->operands.push_back(result);
}

void iushr(const InstructionInput& input)
{
    vdata value2 = input.thread->m_currentFrame->popOperand();
    vdata value1 = input.thread->m_currentFrame->popOperand();

    const vint s = value2.getInt() & 0x1f;
    vint resultVal = (value1.getInt() >> s);

    if (value1.getInt() < 0)
    {
        resultVal = (value1.getInt() >> s) + (2 << ~s) ;
    }

    const vdata result{VariableType_INT, resultVal};
    input.thread->m_currentFrame->operands.push_back(result);
}

void iand(const InstructionInput& input)
{
    const vint val2 = input.thread->m_currentFrame->popInt();
    const vint val1 = input.thread->m_currentFrame->popInt();
    input.thread->m_currentFrame->pushInt(val1&val2);
}

void land(const InstructionInput& input)
{
    StackFrame* currentFrame = input.thread->m_currentFrame;
    const vlong long2 = currentFrame->popLong();
    const vlong long1 = currentFrame->popLong();
    const vlong conjunction = long1 & long2;
    input.thread->m_currentFrame->pushLong(conjunction);
}

void ior(const InstructionInput& input)
{
    const vdata value2 = input.thread->m_currentFrame->popOperand();
    const vdata value1 = input.thread->m_currentFrame->popOperand();

    const vint resultVal = value1.getInt() | value2.getInt();

    const vdata result{VariableType_INT, resultVal};
    input.thread->m_currentFrame->operands.push_back(result);
}

void lor(const InstructionInput& input)
{
    StackFrame* currentFrame = input.thread->m_currentFrame;
    const vlong long2 = currentFrame->popLong();
    const vlong long1 = currentFrame->popLong();
    const vlong conjunction = long1 | long2;
    input.thread->m_currentFrame->pushLong(conjunction);
}

void ixor(const InstructionInput& input)
{
    const vdata value2 = input.thread->m_currentFrame->popOperand();
    const vdata value1 = input.thread->m_currentFrame->popOperand();

    const vint resultVal = value1.getInt() ^ value2.getInt();

    const vdata result{VariableType_INT, resultVal};
    input.thread->m_currentFrame->operands.push_back(result);
}

void iinc(const InstructionInput& input)
{
    const u1 index = input.args[0];
    const i1* argsArr = ((i1*)input.args);
    const i1 constData = argsArr[1];
    vdata* var =  &input.thread->m_currentFrame->localVariables[index];
    var->value.i += constData;
}
