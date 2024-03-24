#pragma once

#include "Library/NativeDefs.h"

JCALL void lib_java_lang_Class_registerNatives(const NativeArgs& args);
JCALL void lib_java_lang_Class_getPrimitiveClass(const NativeArgs& args);
JCALL void lib_java_lang_Class_desiredAssertionStatus0(const NativeArgs& args);
JCALL void lib_java_lang_Class_getName0(const NativeArgs& args);
JCALL void lib_java_lang_Class_forName0(const NativeArgs& args);
JCALL void lib_java_lang_Class_getDeclaredFields0(const NativeArgs& args);
JCALL void lib_java_lang_Class_isPrimitive(const NativeArgs& args);
JCALL void lib_java_lang_Class_isAssignableFrom(const NativeArgs& args);
JCALL void lib_java_lang_Class_isInterface(const NativeArgs& args);
JCALL void lib_java_lang_Class_getDeclaredConstructors0(const NativeArgs& args);
JCALL void lib_java_lang_Class_getModifiers(const NativeArgs& args);
JCALL void lib_java_lang_Class_getSuperClass(const NativeArgs& args);