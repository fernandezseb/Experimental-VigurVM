#pragma once

#include "DynamicArray.h"
#include "VM.h"
#include "JavaHeap.h"
#include "VMThread.h"

struct NativeArgs
{
    VMThread* thread;

    [[nodiscard]] const Object* getObject(const int argIndex) const
    {
        const StackFrame* currentFrame = thread->m_currentFrame;
        const Variable var = currentFrame->localVariables[argIndex];
        VM::get()->checkType(var, VariableType_REFERENCE, thread);
        return VM::get()->getHeap()->getObject(var.data);
    }

    [[nodiscard]] const Object* getThisObjectReference() const
    {
        return getObject(0);
    }

    [[nodiscard]] i4 getInt(const int argIndex) const
    {
        const StackFrame* currentFrame = thread->m_currentFrame;
        const Variable intVar = currentFrame->localVariables[argIndex];
        VM::get()->checkType(intVar, VariableType_INT, thread);
        return static_cast<i4>(intVar.data);
    }

    [[nodiscard]] const Array* getArray(const int argIndex) const
    {
        const StackFrame* currentFrame = thread->m_currentFrame;
        const Variable arrayVar = currentFrame->localVariables[argIndex];
        VM::get()->checkType(arrayVar, VariableType_REFERENCE, thread);
        const Array* array = VM::get()->getHeap()->getArray(arrayVar.data);
        return array;
    }
};

typedef void (*nativeImplementation)(const NativeArgs& args);

struct NativeMethod
{
    const char* name{};
    const char* descriptor{};
    nativeImplementation impl;
    NativeMethod(const char* name, const char* descriptor, nativeImplementation impl)
        : name(name), descriptor(descriptor), impl(impl)
    {}
};

class Native
{
public:
    DynamicArray<NativeMethod> nativeMethods{200};
};

CCALL void registerNative(const char* name,
                        const char* descriptor,
                        void (*nativeImplementation)(const NativeArgs& args));

CCALL nativeImplementation findNativeMethod(const char* name,
    const char* descriptor);

CCALL Native native;