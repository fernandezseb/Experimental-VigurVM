#pragma once

#include "Library/NativeDefs.h"

JCALL void lib_sun_misc_Unsafe_registerNatives(const NativeArgs& args);
JCALL void lib_sun_misc_Unsafe_arrayBaseOffset(vreference reference, const NativeArgs& args);
JCALL void lib_sun_misc_Unsafe_arrayIndexScale(vreference reference, const NativeArgs& args);
JCALL void lib_sun_misc_Unsafe_addressSize(const NativeArgs& args);
JCALL void lib_sun_misc_Unsafe_objectFieldOffset(vreference reference, const NativeArgs& args);
JCALL void lib_sun_misc_Unsafe_compareAndSwapObject(const NativeArgs& args);
JCALL void lib_sun_misc_Unsafe_compareAndSwapInt(const NativeArgs& args);
JCALL void lib_sun_misc_Unsafe_getIntVolatile(const NativeArgs& args);
JCALL void lib_sun_misc_Unsafe_allocateMemory(vvalue value, const NativeArgs& args);
JCALL void lib_sun_misc_Unsafe_freeMemory(vvalue value, const NativeArgs& args);
JCALL void lib_sun_misc_Unsafe_putLong(const NativeArgs& args);
JCALL void lib_sun_misc_Unsafe_getByte(vvalue value, const NativeArgs& args);