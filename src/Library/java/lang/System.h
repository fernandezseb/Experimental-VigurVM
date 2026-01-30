#pragma once

#include "Library/NativeDefs.h"

JCALL void lib_java_lang_System_registerNatives(const NativeArgs& args);
JCALL void lib_java_lang_System_arraycopy(const NativeArgs& args);
JCALL void lib_java_lang_System_initProperties(vreference reference, const NativeArgs& args);
JCALL void lib_java_lang_System_setIn0(vreference reference, const NativeArgs& args);
JCALL void lib_java_lang_System_setOut0(vreference reference, const NativeArgs& args);
JCALL void lib_java_lang_System_setErr0(vreference reference, const NativeArgs& args);
JCALL void lib_java_lang_System_currentTimeMillis(const NativeArgs& args);
JCALL void lib_java_lang_System_mapLibraryName(vreference reference, const NativeArgs& args);