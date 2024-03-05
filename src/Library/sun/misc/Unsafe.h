#pragma once

#include "Library/NativeDefs.h"

JCALL void lib_sun_misc_Unsafe_registerNatives(NATIVE_ARGS);
JCALL void lib_sun_misc_Unsafe_arrayBaseOffset(NATIVE_ARGS);
JCALL void lib_sun_misc_Unsafe_arrayIndexScale(NATIVE_ARGS);
JCALL void lib_sun_misc_Unsafe_addressSize(NATIVE_ARGS);
JCALL void lib_sun_misc_Unsafe_objectFieldOffset(NATIVE_ARGS);
JCALL void lib_sun_misc_Unsafe_compareAndSwapObject(NATIVE_ARGS);
JCALL void lib_sun_misc_Unsafe_compareAndSwapInt(NATIVE_ARGS);
JCALL void lib_sun_misc_Unsafe_getIntVolatile(NATIVE_ARGS);