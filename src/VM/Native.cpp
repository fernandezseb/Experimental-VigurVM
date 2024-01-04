/*
 * Copyright (c) 2023-2024 Sebastiaan Fernandez.
 *
 * This file is part of VigurVM.
 *
 * VigurVM is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * VigurVM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with Foobar.
 * If not, see <https://www.gnu.org/licenses/>.
 */

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
