#include "VMThread.h"

void VMThread::pushStackFrameWithoutParams(ClassInfo* classInfo, const MethodInfo* methodInfo)
{
    StackFrame stackFrame;
    stackFrame.localVariables.reserve(methodInfo->code->maxLocals);
    for (u2  currentLocal = 0; currentLocal < methodInfo->code->maxLocals; ++currentLocal)
    {
        constexpr Variable var{VariableType_UNDEFINED};
        stackFrame.localVariables.push_back(var);
    }
    stackFrame.operands.reserve(methodInfo->code->maxStack);
    stackFrame.constantPool = classInfo->constantPool;
    stackFrame.previousPc = this->pc;
    stackFrame.previousClass = this->currentClass;
    stackFrame.previousMethod = this->currentMethod;
    stackFrame.className = classInfo->getName();
    stackFrame.methodName = methodInfo->name;


    this->pc = 0;
    this->currentClass = classInfo;
    this->currentMethod = methodInfo;

    this->stack.frames.push_back(stackFrame);
    this->currentFrame = &this->stack.frames[this->stack.frames.size()-1];
}

void VMThread::pushStackFrameVirtual(ClassInfo* classInfo, const MethodInfo* methodInfo, StackFrame* previousFrame)
{
    pushStackFrameWithoutParams(classInfo, methodInfo);
    if (previousFrame != nullptr)
    {
        // The arguments and the pointer to the object
        for (int i = methodInfo->argsCount; i >= 0; --i)
        {
            this->currentFrame->localVariables[i] = previousFrame->popOperand();
        }

        const Variable ref = this->currentFrame->localVariables[0];
        if (ref.type == VariableType_REFERENCE && ref.data == 0)
        {
            internalError("NullpointerException in virtual call");
        }
    }
}

void VMThread::internalError(const char* error) const
{
    fprintf(stdout, "Unhandled VM error in thread \"%s\": %s\n", name, error);
    if (!stack.frames.empty())
    {
        for (i8 currentFrame = stack.frames.size() - 1; currentFrame >= 0; --currentFrame)
        {
            const StackFrame frame = stack.frames[currentFrame];
            printf("    at %s.%s\n", frame.className, frame.methodName);
        }
    }
    Platform::exitProgram(6);
}

void VMThread::pushStackFrameStatic(ClassInfo* classInfo, MethodInfo* methodInfo, StackFrame* previousFrame)
{
    pushStackFrameWithoutParams(classInfo, methodInfo);

    if (previousFrame != nullptr)
    {
        // The arguments
        for (int i = methodInfo->argsCount; i > 0; --i)
        {
            this->currentFrame->localVariables[i-1] = previousFrame->popOperand();
        }
    }
}
