#pragma once

#include "Library/NativeDefs.h"

JCALL void lib_java_lang_ClassLoader_registerNatives(const NativeArgs& args);
JCALL void lib_java_lang_ClassLoader_findBuiltinLib(const NativeArgs& args);
JCALL void lib_java_lang_ClassLoader$NativeLibrary_load(const NativeArgs& args);