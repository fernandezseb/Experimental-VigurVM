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

#include "MathInstructions.h"

#include <bit>

#include "VM/VM.h"
#include "Data/Variable.h"

void iadd(INSTRUCTION_ARGS)
{
    const i4 val2 = thread->m_currentFrame->popInt();
    const i4 val1 = thread->m_currentFrame->popInt();
    thread->m_currentFrame->pushInt(val1+val2);
}

void ladd(INSTRUCTION_ARGS)
{
    StackFrame* currentFrame = thread->m_currentFrame;
    const i8 long2 = currentFrame->popLong();
    const i8 long1 = currentFrame->popLong();
    const i8 sum = long1 + long2;
    thread->m_currentFrame->pushLong(sum);
}

void isub(INSTRUCTION_ARGS)
{
    const i4 val2 = thread->m_currentFrame->popInt();
    const i4 val1 = thread->m_currentFrame->popInt();
    thread->m_currentFrame->pushInt(val1-val2);
}

void imul(INSTRUCTION_ARGS)
{
    const i4 val2 = thread->m_currentFrame->popInt();
    const i4 val1 = thread->m_currentFrame->popInt();
    thread->m_currentFrame->pushInt(val1*val2);
}

void fmul(INSTRUCTION_ARGS)
{
    const Variable var2 = thread->m_currentFrame->popOperand();
    const Variable var1 = thread->m_currentFrame->popOperand();
    const Variable added{VariableType_FLOAT,
        std::bit_cast<u4>(std::bit_cast<float>(var1.data)
            * std::bit_cast<float>(var2.data))};
    thread->m_currentFrame->operands.push_back(added);
}

void irem(INSTRUCTION_ARGS)
{
    const i4 val2 = thread->m_currentFrame->popInt();
    const i4 val1 = thread->m_currentFrame->popInt();
    const i4 result = val1%val2;
    // TODO: Fix for edge cases
    thread->m_currentFrame->pushInt(result);
}

void lshl(INSTRUCTION_ARGS)
{
    StackFrame* currentFrame = thread->m_currentFrame;
    const i4 int2 = currentFrame->popInt();
    const i8 long1 = currentFrame->popLong();
    const i8 result = long1 << (int2 & 0x3f);
    thread->m_currentFrame->pushLong(result);
}

void ishl(INSTRUCTION_ARGS)
{
    const Variable value2 = thread->m_currentFrame->popOperand();
    const Variable value1 = thread->m_currentFrame->popOperand();

    const i4 s = ((i4)value2.data) & 0x1f;
    const i4 resultVal = value1.data << s;
    const Variable result{VariableType_INT,
        std::bit_cast<u4>(resultVal)};

    thread->m_currentFrame->operands.push_back(result);
}

void iushr(INSTRUCTION_ARGS)
{
    Variable value2 = thread->m_currentFrame->popOperand();
    Variable value1 = thread->m_currentFrame->popOperand();

    const i4 s = ((i4)value2.data) & 0x1f;
    i4 resultVal = ((i4)value1.data >> s);

    if ((i4)value1.data < 0)
    {
        resultVal = ((i4)value1.data >> s) + (2 << ~s) ;
    }

    const Variable result{VariableType_INT,
        std::bit_cast<u4>(resultVal)};
    thread->m_currentFrame->operands.push_back(result);
}

void iand(INSTRUCTION_ARGS)
{
    const i4 val2 = thread->m_currentFrame->popInt();
    const i4 val1 = thread->m_currentFrame->popInt();
    thread->m_currentFrame->pushInt(val1&val2);
}

void land(INSTRUCTION_ARGS)
{
    StackFrame* currentFrame = thread->m_currentFrame;
    const i8 long2 = currentFrame->popLong();
    const i8 long1 = currentFrame->popLong();
    const i8 conjunction = long1 & long2;
    thread->m_currentFrame->pushLong(conjunction);
}

void ior(INSTRUCTION_ARGS)
{
    const Variable value2 = thread->m_currentFrame->popOperand();
    const Variable value1 = thread->m_currentFrame->popOperand();

    const u4 resultVal = value1.data | value2.data;

    const Variable result{VariableType_INT,
        std::bit_cast<u4>(resultVal)};
    thread->m_currentFrame->operands.push_back(result);
}

void ixor(INSTRUCTION_ARGS)
{
    const Variable value2 = thread->m_currentFrame->popOperand();
    const Variable value1 = thread->m_currentFrame->popOperand();

    const u4 resultVal = value1.data ^ value2.data;

    const Variable result{VariableType_INT,
        std::bit_cast<u4>(resultVal)};
    thread->m_currentFrame->operands.push_back(result);
}

void iinc(INSTRUCTION_ARGS)
{
    const u1 index = args[0];
    const i1* argsArr = ((i1*)args);
    const i1 constData = argsArr[1];
    Variable* var =  &thread->m_currentFrame->localVariables[index];
    var->data += constData;
}
