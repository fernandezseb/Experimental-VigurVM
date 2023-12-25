#pragma once

#include "DynamicArray.h"
#include "VM.h"
#include "JavaHeap.h"
#include "VMThread.h"

typedef void (*nativeImplementation)(JavaHeap* heap, VMThread* thread, VM* VM);

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
                        void (*nativeImplementation)(JavaHeap* heap, VMThread* thread, VM* VM));

CCALL nativeImplementation findNativeMethod(const char* name,
    const char* descriptor);

CCALL Native native;