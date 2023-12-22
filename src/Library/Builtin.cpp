#include "Builtin.h"

#include "java/lang/Object.h"
#include "java/lang/System.h"
#include "Vigur/lang/System.h"

void registerBuiltinRegisterNatives()
{
    // java/lang package
    registerNative("java/lang/Object/registerNatives", "()V", lib_java_lang_Object_registerNatives);
    registerNative("java/lang/System/registerNatives", "()V", lib_java_lang_System_registerNatives);
    // Vigur/lang packge
    registerNative("Vigur/lang/System/registerNatives", "()V", lib_Vigur_lang_System_registerNatives);
}
