#include "Builtin.h"

#include "java/io/FileDescriptor.h"
#include "java/io/FileOutputStream.h"
#include "java/lang/Object.h"
#include "java/lang/System.h"
#include "sun/misc/Unsafe.h"
#include "Vigur/lang/System.h"

void registerBuiltinRegisterNatives()
{
    // java/lang package
    registerNative("java/lang/Object/registerNatives", "()V", lib_java_lang_Object_registerNatives);
    registerNative("java/lang/System/registerNatives", "()V", lib_java_lang_System_registerNatives);
    // java/io package
    registerNative("java/io/FileOutputStream/initIDs", "()V", lib_java_io_FileOutputStream_initIDs);
    registerNative("java/io/FileDescriptor/initIDs", "()V", lib_java_io_FileDescriptor_initIDs);
    // sun/misc
    registerNative("sun/misc/Unsafe/registerNatives", "()V", lib_sun_misc_Unsafe_registerNatives);
    // Vigur/lang package
    registerNative("Vigur/lang/System/registerNatives", "()V", lib_Vigur_lang_System_registerNatives);
}
