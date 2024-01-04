#include "MathInstructions.h"

#include <bit>

#include "VM/VM.h"
#include "Data/Variable.h"

void iadd(INSTRUCTION_ARGS)
{
    // TODO: Check types and convert to int correctly
    const Variable var1 = thread->m_currentFrame->popOperand();
    const Variable var2 = thread->m_currentFrame->popOperand();
    const Variable added{VariableType_INT,
        std::bit_cast<u4>((i4)var1.data + (i4)var2.data)};
    thread->m_currentFrame->operands.push_back(added);
}

void isub(INSTRUCTION_ARGS)
{
    // TODO: Check types and convert to int correctly
    const Variable var2 = thread->m_currentFrame->popOperand();
    const Variable var1 = thread->m_currentFrame->popOperand();
    const Variable added{VariableType_INT,
        std::bit_cast<u4>((i4)var1.data - (i4)var2.data)};
    thread->m_currentFrame->operands.push_back(added);
}

void imul(INSTRUCTION_ARGS)
{
    const Variable var2 = thread->m_currentFrame->popOperand();
    const Variable var1 = thread->m_currentFrame->popOperand();
    const Variable added{VariableType_INT,
        std::bit_cast<u4>((i4)var1.data * (i4)var2.data)};
    thread->m_currentFrame->operands.push_back(added);
}

void ishl(INSTRUCTION_ARGS)
{
    Variable value2 = thread->m_currentFrame->popOperand();
    Variable value1 = thread->m_currentFrame->popOperand();

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
    const Variable value2 = thread->m_currentFrame->popOperand();
    const Variable value1 = thread->m_currentFrame->popOperand();
    const u4 resultVal = value1.data & value2.data;
    const Variable result{VariableType_INT,
        std::bit_cast<u4>(resultVal)};
    thread->m_currentFrame->operands.push_back(result);
}

void ixor(INSTRUCTION_ARGS)
{
    const Variable value2 = thread->m_currentFrame->popOperand();
    const Variable value1 = thread->m_currentFrame->popOperand();

    const u4 resultVal = value1.data | value2.data;

    const Variable result{VariableType_INT,
        std::bit_cast<u4>(resultVal)};
    thread->m_currentFrame->operands.push_back(result);
}

void iinc(INSTRUCTION_ARGS)
{
    u1 index = args[0];
    i1* argsArr = ((i1*)args);
    i1 constData = argsArr[1];
    Variable* var =  &thread->m_currentFrame->localVariables[index];
    var->data += constData;
}
