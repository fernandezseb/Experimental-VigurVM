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