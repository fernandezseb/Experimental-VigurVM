#pragma once

#include "Core.h"
#include "JavaHeap.h"
#include "JavaStack.h"
#include "Data/Class.h"

class VMThread
{
public:
    u4 pc{0};
    JavaStack stack;
    // Current frame
    StackFrame* currentFrame{nullptr};
    // Current method
    const MethodInfo* currentMethod{nullptr};
    // current class
    ClassInfo* currentClass{nullptr};
    const char* name{nullptr};

    explicit VMThread(const char* name, const size_t frameSize) noexcept
        : name(name), stack(frameSize)
    {
    }
    void pushStackFrameWithoutParams(ClassInfo* classInfo, const MethodInfo* methodInfo);
    void pushStackFrameStatic(ClassInfo* classInfo, MethodInfo* methodInfo, StackFrame* previousFrame);
    void pushStackFrameVirtual(ClassInfo* classInfo, const MethodInfo* methodInfo, StackFrame* previousFrame, JavaHeap* heap);
    void pushStackFrameSpecial(ClassInfo* classInfo, const MethodInfo* methodInfo, StackFrame* previousFrame, JavaHeap* heap);
    void internalError(const char* error) const;
};
