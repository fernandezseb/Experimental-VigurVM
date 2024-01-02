#pragma once

#include <stack>

#include "Core.h"
#include "JavaHeap.h"
#include "JavaStack.h"
#include "Data/Class.h"

class VMThread
{
public:
    u4 m_pc{0};
    std::stack<JavaStack> m_stackstack;
    // Current frame
    StackFrame* m_currentFrame{nullptr};
    // Current method
    const MethodInfo* m_currentMethod{nullptr};
    // current class
    ClassInfo* m_currentClass{nullptr};
    const char* m_name{nullptr};

    explicit VMThread(const char* name, const size_t frameSize) noexcept
        : m_name(name)
    {
        m_stackstack.emplace(frameSize);
    }
    void pushStackFrameWithoutParams(ClassInfo* classInfo, const MethodInfo* methodInfo);
    void pushStackFrameStatic(ClassInfo* classInfo, MethodInfo* methodInfo, StackFrame* previousFrame);
    void pushStackFrameSpecial(ClassInfo* classInfo, const MethodInfo* methodInfo, StackFrame* previousFrame, JavaHeap* heap);
    void internalError(const char* error);
};
