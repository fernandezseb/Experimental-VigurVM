#include "MathInstructions.h"

#include <bit>

#include "VM/VM.h"
#include "Data/Variable.h"

void iadd(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    // TODO: Check types and convert to int correctly
    const Variable var1 = thread->currentFrame->popOperand();
    const Variable var2 = thread->currentFrame->popOperand();
    const Variable added{VariableType_INT,
        std::bit_cast<u4>((i4)var1.data + (i4)var2.data)};
    thread->currentFrame->operands.push_back(added);
    thread->internalError("test exception");
}

void isub(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    // TODO: Check types and convert to int correctly
    const Variable var2 = thread->currentFrame->popOperand();
    const Variable var1 = thread->currentFrame->popOperand();
    const Variable added{VariableType_INT,
        std::bit_cast<u4>((i4)var1.data - (i4)var2.data)};
    thread->currentFrame->operands.push_back(added);
}

void imul(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    const Variable var2 = thread->currentFrame->popOperand();
    const Variable var1 = thread->currentFrame->popOperand();
    const Variable added{VariableType_INT,
        std::bit_cast<u4>((i4)var1.data * (i4)var2.data)};
    thread->currentFrame->operands.push_back(added);
}

void ishl(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    Variable value2 = thread->currentFrame->popOperand();
    Variable value1 = thread->currentFrame->popOperand();

    const i4 s = ((i4)value2.data) & 0x1f;
    const i4 resultVal = value1.data << s;
    const Variable result{VariableType_INT,
        std::bit_cast<u4>(resultVal)};

    thread->currentFrame->operands.push_back(result);
}

void iushr(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    Variable value2 = thread->currentFrame->popOperand();
    Variable value1 = thread->currentFrame->popOperand();

    const i4 s = ((i4)value2.data) & 0x1f;
    i4 resultVal = ((i4)value1.data >> s);

    if ((i4)value1.data < 0)
    {
        resultVal = ((i4)value1.data >> s) + (2 << ~s) ;
    }

    const Variable result{VariableType_INT,
        std::bit_cast<u4>(resultVal)};
    thread->currentFrame->operands.push_back(result);
}

void iinc(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    u1 index = args[0];
    i1* argsArr = ((i1*)args);
    i1 constData = argsArr[1];
    Variable* var =  &thread->currentFrame->localVariables[index];
    var->data += constData;
}
