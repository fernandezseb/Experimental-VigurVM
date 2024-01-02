#include "VMThread.h"

#include "JavaHeap.h"

#include <stack>

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
    stackFrame.previousPc = m_pc;
    stackFrame.previousClass = m_currentClass;
    stackFrame.previousMethod = m_currentMethod;
    stackFrame.className = classInfo->getName();
    stackFrame.methodName = methodInfo->name;


    m_pc = 0;
    m_currentClass = classInfo;
    m_currentMethod = methodInfo;

    this->m_stackstack.top().frames.push_back(stackFrame);
    m_currentFrame = &this->m_stackstack.top().frames[this->m_stackstack.top().frames.size()-1];
}


void VMThread::pushStackFrameSpecial(ClassInfo* classInfo, const MethodInfo* methodInfo, StackFrame* previousFrame,
    [[maybe_unused]] JavaHeap* heap)
{
    std::deque<Variable> arguments;
    if (previousFrame != nullptr)
    {
        // The arguments and the pointer to the object
        for (int i = methodInfo->argsCount; i >= 0; --i)
        {
            arguments.push_front(previousFrame->popOperand());
        }

        const Variable ref = arguments[0];
        if (ref.type == VariableType_REFERENCE && ref.data == 0)
        {
            internalError("NullpointerException in special call");
        }
    }

    pushStackFrameWithoutParams(classInfo, methodInfo);
    if (!arguments.empty())
    {
        for (int i = 0; i <= methodInfo->argsCount; ++i)
        {
            m_currentFrame->localVariables[i] = arguments[i];
        }
    }
}

void VMThread::internalError(const char* error)
{
    fprintf(stdout, "Unhandled VM error in thread \"%s\": %s\n", m_name, error);
    while (!m_stackstack.empty()) {
        if (!m_stackstack.top().frames.empty())
        {
            for (i8 currentFrame = m_stackstack.top().frames.size() - 1; currentFrame >= 0; --currentFrame)
            {
                const StackFrame frame = m_stackstack.top().frames[currentFrame];
                printf("    at %s.%s\n", frame.className, frame.methodName);
            }
        }
        m_stackstack.pop();
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
            m_currentFrame->localVariables[i-1] = previousFrame->popOperand();
        }
    }
}
