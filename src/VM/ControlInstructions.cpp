#include "ControlInstructions.h"

#include "VM.h"
#include "Data/Variable.h"

void returnInstruction(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
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
