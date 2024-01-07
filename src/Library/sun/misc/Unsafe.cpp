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

#include "Unsafe.h"

#include <bit>

JCALL void lib_sun_misc_Unsafe_registerNatives(NATIVE_ARGS)
{
    printf("[Running sun/misc/Unsafe/registerNatives()V]\n");
    registerNative("sun/misc/Unsafe/arrayBaseOffset", "(Ljava/lang/Class;)I", lib_sun_misc_Unsafe_arrayBaseOffset);
    registerNative("sun/misc/Unsafe/arrayIndexScale", "(Ljava/lang/Class;)I", lib_sun_misc_Unsafe_arrayIndexScale);
    registerNative("sun/misc/Unsafe/addressSize", "()I", lib_sun_misc_Unsafe_addressSize);
}

JCALL void lib_sun_misc_Unsafe_arrayBaseOffset(NATIVE_ARGS)
{
    constexpr u4 offset = offsetof(Array, data);
    constexpr u4 val = std::bit_cast<u4>(offset);
    thread->returnVar(Variable{VariableType_INT,  val});
}

JCALL void lib_sun_misc_Unsafe_arrayIndexScale(NATIVE_ARGS)
{
    const Variable classObjectRef = thread->m_currentFrame->localVariables[1];
    const ClassObject* classObject = heap->getClassObject(classObjectRef.data);
    u4 length = 4;
    if (classObject->name[1] == 'C')
    {
        length = 1;
    } else if (classObject->name[1] == 'D' || classObject->name[1] == 'J')
    {
        length = 8;
    }
    thread->returnVar(Variable{VariableType_INT, length});
}

JCALL void lib_sun_misc_Unsafe_addressSize(NATIVE_ARGS)
{
    constexpr Variable size{VariableType_INT, 64};
    thread->returnVar(size);
}
