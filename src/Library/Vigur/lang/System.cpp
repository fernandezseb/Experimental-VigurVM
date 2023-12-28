#include "System.h"

JCALL void lib_vigur_lang_System_printLnString(NATIVE_ARGS)
{
    Variable strVar = thread->currentFrame->popOperand();
    Object* obj = heap->getObject(strVar.data);
    FieldData* charArrRef = obj->getField("value", "[C", heap);
    Array* charArr = heap->getArray(charArrRef->data[0].data);
    Platform::print((const char*)charArr->data, charArr->length);
    Platform::print("\n", 1);
    Platform::flush();
}

JCALL void lib_Vigur_lang_System_registerNatives(NATIVE_ARGS)
{
    registerNative("Vigur/lang/System/printLn", "(Ljava/lang/String;)V", lib_vigur_lang_System_printLnString);
}
