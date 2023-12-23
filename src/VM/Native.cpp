#include "Native.h"

Native native;

CCALL void registerNative(const char* name, const char* descriptor,
    void(* nativeImplementation)(JavaHeap* heap, VMThread* thread, VM* VM))
{
    const NativeMethod nativeMethod{name, descriptor, nativeImplementation};
    native.nativeMethods.add(nativeMethod);
}

CCALL nativeImplementation findNativeMethod(const char* name, const char* descriptor)
{
    for (size_t currentMethod = 0; currentMethod < native.nativeMethods.getSize(); ++currentMethod)
    {
        const NativeMethod method = native.nativeMethods.get(currentMethod);
        if (strcmp(method.name, name) == 0
            && strcmp(method.descriptor, descriptor) == 0)
        {
            return method.impl;
        }
    }
    return nullptr;
}
