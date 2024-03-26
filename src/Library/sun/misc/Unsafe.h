#pragma once

#include "Library/NativeDefs.h"

JCALL void lib_sun_misc_Unsafe_registerNatives(const NativeArgs& args);
JCALL void lib_sun_misc_Unsafe_arrayBaseOffset(const NativeArgs& args);
JCALL void lib_sun_misc_Unsafe_arrayIndexScale(const NativeArgs& args);
JCALL void lib_sun_misc_Unsafe_addressSize(const NativeArgs& args);
JCALL void lib_sun_misc_Unsafe_objectFieldOffset(const NativeArgs& args);
JCALL void lib_sun_misc_Unsafe_compareAndSwapObject(const NativeArgs& args);
JCALL void lib_sun_misc_Unsafe_compareAndSwapInt(const NativeArgs& args);
JCALL void lib_sun_misc_Unsafe_getIntVolatile(const NativeArgs& args);
JCALL void lib_sun_misc_Unsafe_allocateMemory(const NativeArgs& args);
JCALL void lib_sun_misc_Unsafe_freeMemory(const NativeArgs& args);
JCALL void lib_sun_misc_Unsafe_putLong(const NativeArgs& args);
JCALL void lib_sun_misc_Unsafe_getByte(const NativeArgs& args);