#pragma once

#include "DynamicArray.h"
#include "VM.h"
#include "JavaHeap.h"
#include "VMThread.h"

typedef void (*nativeImplementation)(JavaHeap* heap, VMThread* thread, VM* VM);
typedef nativeImplementation (*nativeDef)();


struct NativeMethod
{
public:
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

extern "C" void registerNative(const char* name,
                        const char* descriptor,
                        void (*nativeImplementation)(JavaHeap* heap, VMThread* thread, VM* VM));

extern "C" nativeImplementation findNativeMethod(const char* name,
    const char* descriptor);

extern "C" extern Native native;