#pragma once

#include "Library/NativeDefs.h"

JCALL void lib_java_lang_System_registerNatives(const NativeArgs& args);
JCALL void lib_java_lang_System_arraycopy(const NativeArgs& args);
JCALL void lib_java_lang_System_initProperties(const NativeArgs& args);
JCALL void lib_java_lang_System_setIn0(const NativeArgs& args);
JCALL void lib_java_lang_System_setOut0(const NativeArgs& args);
JCALL void lib_java_lang_System_setErr0(const NativeArgs& args);