#pragma once

#include "DynamicArray.h"
#include "VM.h"
#include "JavaHeap.h"
#include "VMThread.h"

struct NativeArgs
{
    JavaHeap* heap;
    VMThread* thread;
    VM* vm;

    [[nodiscard]] const Object* getThisObjectReference() const
    {
        const StackFrame* currentFrame = thread->m_currentFrame;
        const Variable var = currentFrame->localVariables[0];
        vm->checkType(var, VariableType_REFERENCE, thread);
        return heap->getObject(var.data);
    }

    [[nodiscard]] i4 getInt(const int argIndex) const
    {
        const StackFrame* currentFrame = thread->m_currentFrame;
        const Variable intVar = currentFrame->localVariables[argIndex];
        vm->checkType(intVar, VariableType_INT, thread);
        return static_cast<i4>(intVar.data);
    }

    [[nodiscard]] const Array* getArray(const int argIndex) const
    {
        const StackFrame* currentFrame = thread->m_currentFrame;
        const Variable arrayVar = currentFrame->localVariables[argIndex];
        vm->checkType(arrayVar, VariableType_REFERENCE, thread);
        const Array* array = heap->getArray(arrayVar.data);
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