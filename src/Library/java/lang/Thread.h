#pragma once

#include "Library/NativeDefs.h"

JCALL void lib_java_lang_Thread_registerNatives(const NativeArgs& args);
JCALL void lib_java_lang_Thread_currentThread(const NativeArgs& args);
JCALL void lib_java_lang_Thread_setPriority0(const NativeArgs& args);
JCALL void lib_java_lang_Thread_isAlive(const NativeArgs& args);
JCALL void lib_java_lang_Thread_start0(const NativeArgs& args);