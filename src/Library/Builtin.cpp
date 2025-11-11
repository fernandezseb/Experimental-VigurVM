/*
 * Copyright (c) 2023-2025 Sebastiaan Fernandez.
 *
 * This file is part of VigurVM.
 *
 * VigurVM is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * VigurVM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with VigurVM.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "Builtin.h"

#include "java/io/FileDescriptor.h"
#include "java/io/FileOutputStream.h"
#include "java/io/FileInputStream.h"
#include "java/lang/Class.h"
#include "java/lang/ClassLoader.h"
#include "java/lang/Double.h"
#include "java/lang/Float.h"
#include "java/lang/Object.h"
#include "java/lang/String.h"
#include "java/lang/System.h"
#include "java/lang/Thread.h"
#include "java/lang/Throwable.h"
#include "java/security/AccessController.h"
#include "java/util/concurrent/atomic/AtomicLong.h"
#include "sun/misc/Unsafe.h"
#include "sun/misc/VM.h"
#include "sun/reflect/Reflection.h"
#include "sun/reflect/NativeConstructorAccessorImpl.h"
#include "vigur/lang/System.h"

void registerBuiltinRegisterNatives()
{
    // java/lang package
    registerNative("java/lang/Object/registerNatives", "()V", lib_java_lang_Object_registerNatives);
    registerNative("java/lang/Class/registerNatives", "()V", lib_java_lang_Class_registerNatives);
    registerNative("java/lang/System/registerNatives", "()V", lib_java_lang_System_registerNatives);
    registerNative("java/lang/Float/floatToRawIntBits", "(F)I", lib_java_lang_Float_floatToRawIntBits);
    registerNative("java/lang/Double/doubleToRawLongBits", "(D)J", lib_java_lang_Double_doubleToRawLongBits);
    registerNative("java/lang/Double/longBitsToDouble", "(J)D", lib_java_lang_Double_longBitsToDouble);
    registerNative("java/lang/Thread/registerNatives", "()V", lib_java_lang_Thread_registerNatives);
    registerNative("java/lang/String/intern", "()Ljava/lang/String;", lib_java_lang_String_intern);
    registerNative("java/lang/Throwable/fillInStackTrace", "(I)Ljava/lang/Throwable;", lib_java_lang_Throwable_fillInStackTrace);
    registerNative("java/util/concurrent/atomic/AtomicLong/VMSupportsCS8", "()Z", lib_java_util_concurrent_atomic_AtomicLong_VMSupportsCS8);
    registerNative("java/lang/ClassLoader/registerNatives", "()V", lib_java_lang_ClassLoader_registerNatives);
    // java/io package
    registerNative("java/io/FileOutputStream/initIDs", "()V", lib_java_io_FileOutputStream_initIDs);
    registerNative("java/io/FileInputStream/initIDs", "()V", lib_java_io_FileInputStream_initIDs);
    registerNative("java/io/FileDescriptor/initIDs", "()V", lib_java_io_FileDescriptor_initIDs);
    registerNative("java/io/FileDescriptor/set", "(I)J", lib_java_io_FileDescriptor_set);
    // sun/misc
    registerNative("sun/misc/Unsafe/registerNatives", "()V", lib_sun_misc_Unsafe_registerNatives);
    registerNative("sun/misc/VM/initialize", "()V", lib_sun_misc_VM_initialize);
    // Security API
    registerNative("java/security/AccessController/doPrivileged", "(Ljava/security/PrivilegedAction;)Ljava/lang/Object;", lib_java_security_AccessController_doPriviliged);
    registerNative("java/security/AccessController/doPrivileged", "(Ljava/security/PrivilegedExceptionAction;)Ljava/lang/Object;", lib_java_security_AccessController_doPriviliged_PriviligedExceptionAction);
    registerNative("java/security/AccessController/getStackAccessControlContext", "()Ljava/security/AccessControlContext;", lib_java_security_AccessController_getStackAccessControlContext);
    // Vigur/lang package
    registerNative("vigur/lang/System/registerNatives", "()V", lib_vigur_lang_System_registerNatives);
    registerNative("vigur/lang/System/printLnString", "(Ljava/lang/String;)V", lib_vigur_lang_System_printLnString);
    // Reflection API
    registerNative("sun/reflect/Reflection/getCallerClass", "()Ljava/lang/Class;", lib_sun_reflect_Reflection_getCallerClass);
    registerNative("sun/reflect/Reflection/getClassAccessFlags", "(Ljava/lang/Class;)I", lib_sun_reflect_Reflection_getClassAccessFlags);
    registerNative("sun/reflect/NativeConstructorAccessorImpl/newInstance0", "(Ljava/lang/reflect/Constructor;[Ljava/lang/Object;)Ljava/lang/Object;", lib_sun_reflect_NativeConstructorAccessorImpl_newInstance0);
}
