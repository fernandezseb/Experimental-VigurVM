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

#include "Builtin.h"

#include "java/io/FileDescriptor.h"
#include "java/io/FileOutputStream.h"
#include "java/lang/Class.h"
#include "java/lang/Double.h"
#include "java/lang/Float.h"
#include "java/lang/Object.h"
#include "java/lang/System.h"
#include "sun/misc/Unsafe.h"
#include "sun/misc/VM.h"
#include "sun/reflect/Reflection.h"

void registerBuiltinRegisterNatives()
{
    // java/lang package
    registerNative("java/lang/Object/registerNatives", "()V", lib_java_lang_Object_registerNatives);
    registerNative("java/lang/Class/registerNatives", "()V", lib_java_lang_Class_registerNatives);
    registerNative("java/lang/System/registerNatives", "()V", lib_java_lang_System_registerNatives);
    registerNative("java/lang/Float/floatToRawIntBits", "(F)I", lib_java_lang_Float_floatToRawIntBits);
    registerNative("java/lang/Double/doubleToRawLongBits", "(D)J", lib_java_lang_Double_doubleToRawLongBits);
    registerNative("java/lang/Double/longBitsToDouble", "(J)D", lib_java_lang_Double_longBitsToDouble);
    // java/io package
    registerNative("java/io/FileOutputStream/initIDs", "()V", lib_java_io_FileOutputStream_initIDs);
    registerNative("java/io/FileDescriptor/initIDs", "()V", lib_java_io_FileDescriptor_initIDs);
    // sun/misc
    registerNative("sun/misc/Unsafe/registerNatives", "()V", lib_sun_misc_Unsafe_registerNatives);
    registerNative("sun/misc/VM/initialize", "()V", lib_sun_misc_VM_initialize);
    // Vigur/lang package
    // registerNative("Vigur/lang/System/registerNatives", "()V", lib_Vigur_lang_System_registerNatives);
    // Reflection API
    registerNative("sun/reflect/Reflection/getCallerClass", "()Ljava/lang/Class;", lib_sun_reflect_Reflection_getCallerClass);
}
