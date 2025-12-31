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
        const vdata var = currentFrame->localVariables[argIndex];
        return VM::get()->getHeap()->getObject(var.getReference());
    }

    [[nodiscard]] const ClassObject* getClassObject(const int argIndex) const
    {
        const StackFrame* currentFrame = thread->m_currentFrame;
        const vdata var = currentFrame->localVariables[argIndex];
        return VM::get()->getHeap()->getClassObject(var.getReference());
    }

    [[nodiscard]] const Object* getThisObjectReference() const
    {
        return getObject(0);
    }

    [[nodiscard]] const ClassObject* getThisClassObjectReference() const
    {
        return getClassObject(0);
    }

    [[nodiscard]] i4 getInt(const int argIndex) const
    {
        const StackFrame* currentFrame = thread->m_currentFrame;
        const vdata intVar = currentFrame->localVariables[argIndex];
        return intVar.getInt();
    }

    [[nodiscard]] const Array* getArray(const int argIndex) const
    {
        const StackFrame* currentFrame = thread->m_currentFrame;
        const vdata arrayVar = currentFrame->localVariables[argIndex];
        const Array* array = VM::get()->getHeap()->getArray(arrayVar.getReference());
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