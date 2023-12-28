#include "ControlInstructions.h"

#include "VM/VM.h"
#include "Data/Variable.h"

void gotoInstruction(INSTRUCTION_ARGS)
{
    u1 byte1 = args[0];
    u1 byte2 = args[1];

    i2 branchByte = (byte1 << 8) | byte2;

    thread->pc = thread->pc-3+branchByte;
}

void ireturnInstruction(INSTRUCTION_ARGS)
{
    StackFrame* stackFrame = thread->currentFrame;
    thread->pc = stackFrame->previousPc;
    thread->currentClass = stackFrame->previousClass;
    thread->currentMethod = stackFrame->previousMethod;

    Variable returnVal = thread->currentFrame->popOperand();

    thread->stack.frames.pop_back();
    if (thread->stack.frames.size() > 0)
    {
        thread->currentFrame = &thread->stack.frames[thread->stack.frames.size()-1];
        thread->currentFrame->operands.push_back(returnVal);
    } else
    {
        thread->currentFrame = 0;
    }
}

void areturnInstruction(INSTRUCTION_ARGS)
{
    StackFrame* stackFrame = thread->currentFrame;
    thread->pc = stackFrame->previousPc;
    thread->currentClass = stackFrame->previousClass;
    thread->currentMethod = stackFrame->previousMethod;

    Variable returnVal = thread->currentFrame->popOperand();

    thread->stack.frames.pop_back();
    if (thread->stack.frames.size() > 0)
    {
        thread->currentFrame = &thread->stack.frames[thread->stack.frames.size()-1];
        thread->currentFrame->operands.push_back(returnVal);
    } else
    {
        thread->currentFrame = 0;
    }
}

void returnInstruction(INSTRUCTION_ARGS)
{
    StackFrame* stackFrame = thread->currentFrame;
    thread->pc = stackFrame->previousPc;
    thread->currentClass = stackFrame->previousClass;
    thread->currentMethod = stackFrame->previousMethod;
    thread->stack.frames.pop_back();
    if (thread->stack.frames.size() > 0)
    {
        thread->currentFrame = &thread->stack.frames[thread->stack.frames.size()-1];
    } else
    {
        thread->currentFrame = 0;
    }
}
