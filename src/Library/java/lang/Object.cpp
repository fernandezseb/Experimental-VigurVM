#include "Object.h"

JCALL void lib_java_lang_Object_registerNatives(NATIVE_ARGS)
{
    registerNative("java/lang/Object/hashCode", "()I", lib_java_lang_Object_hashCode);
}

JCALL void lib_java_lang_Object_hashCode(JavaHeap* heap, VMThread* thread, VM* VM)
{
    const StackFrame* currentFrame = thread->m_currentFrame;
    const Variable var{VariableType_INT, currentFrame->localVariables[0].data};
    thread->returnVar(var);
}
