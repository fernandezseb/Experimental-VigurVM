#include "ComparisonInstructions.h"

#include "VM/VM.h"
#include "Data/Variable.h"

void ifeq(INSTRUCTION_ARGS)
{
    u1 byte1 = args[0];
    u1 byte2 = args[1];

    i2 branchByte = (byte1 << 8) | byte2;

    Variable intVar = thread->currentFrame->popOperand();
    VM->checkType(intVar, VariableType_INT, thread);
    if (((i4)intVar.data) == 0)
    {
        thread->pc = thread->pc-3+branchByte;
    }
}

void ifne(INSTRUCTION_ARGS)
{
    u1 byte1 = args[0];
    u1 byte2 = args[1];

    i2 branchByte = (byte1 << 8) | byte2;

    Variable intVar = thread->currentFrame->popOperand();
    VM->checkType(intVar, VariableType_INT, thread);

    if (((i4)intVar.data) != 0)
    {
        thread->pc = thread->pc-3+branchByte;
    }
}

void ifge(INSTRUCTION_ARGS)
{
    u1 byte1 = args[0];
    u1 byte2 = args[1];

    i2 branchByte = (byte1 << 8) | byte2;

    Variable intVar = thread->currentFrame->popOperand();
    VM->checkType(intVar, VariableType_INT, thread);

    if (((i4)intVar.data) >= 0)
    {
        thread->pc = thread->pc-3+branchByte;
    }
}

void ifgt(INSTRUCTION_ARGS)
{
    u1 byte1 = args[0];
    u1 byte2 = args[1];

    i2 branchByte = (byte1 << 8) | byte2;

    Variable intVar = thread->currentFrame->popOperand();
    VM->checkType(intVar, VariableType_INT, thread);

    if (((i4)intVar.data) > 0)
    {
        thread->pc = thread->pc-3+branchByte;
    }
}

void ifle(INSTRUCTION_ARGS)
{
    u1 byte1 = args[0];
    u1 byte2 = args[1];

    i2 branchByte = (byte1 << 8) | byte2;

    Variable intVar = thread->currentFrame->popOperand();
    VM->checkType(intVar, VariableType_INT, thread);

    if (((i4)intVar.data) <= 0)
    {
        thread->pc = thread->pc-3+branchByte;
    }
}

void if_icmpne(INSTRUCTION_ARGS)
{
    u1 byte1 = args[0];
    u1 byte2 = args[1];

    i2 branchByte = (byte1 << 8) | byte2;

    Variable var2 = thread->currentFrame->popOperand();
    Variable var1 = thread->currentFrame->popOperand();

    if (((i4)var1.data) != ((i4)var2.data))
    {
        thread->pc = thread->pc-3+branchByte;
    }
}

void if_icmplt(INSTRUCTION_ARGS)
{
    u1 byte1 = args[0];
    u1 byte2 = args[1];

    i2 branchByte = (byte1 << 8) | byte2;

    Variable var2 = thread->currentFrame->popOperand();
    Variable var1 = thread->currentFrame->popOperand();

    if (((i4)var1.data) < ((i4)var2.data))
    {
        thread->pc = thread->pc-3+branchByte;
    }
}

void if_icmpgt(INSTRUCTION_ARGS)
{
    u1 byte1 = args[0];
    u1 byte2 = args[1];

    i2 branchByte = (byte1 << 8) | byte2;

    Variable var2 = thread->currentFrame->popOperand();
    Variable var1 = thread->currentFrame->popOperand();

    if (((i4)var1.data) > ((i4)var2.data))
    {
        thread->pc = thread->pc-3+branchByte;
    }
}

void if_icmple(INSTRUCTION_ARGS)
{
    u1 byte1 = args[0];
    u1 byte2 = args[1];

    i2 branchByte = (byte1 << 8) | byte2;

    Variable var2 = thread->currentFrame->popOperand();
    Variable var1 = thread->currentFrame->popOperand();

    if (((i4)var1.data) <= ((i4)var2.data))
    {
        thread->pc = thread->pc-3+branchByte;
    }
}

void if_acmpne(INSTRUCTION_ARGS)
{
    u1 byte1 = args[0];
    u1 byte2 = args[1];

    i2 branchByte = (byte1 << 8) | byte2;

    Variable var2 = thread->currentFrame->popOperand();
    Variable var1 = thread->currentFrame->popOperand();

    if (((i4)var1.data) != ((i4)var2.data))
    {
        thread->pc = thread->pc-3+branchByte;
    }
}
