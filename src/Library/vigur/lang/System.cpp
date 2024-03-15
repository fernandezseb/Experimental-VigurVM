/*
 * Copyright (c) 2023-2024 Sebastiaan Fernandez.
 *
 * This file is part of VigurVM.
 *
 * VigurVM is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * VigurVM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with Foobar.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "System.h"

JCALL void lib_vigur_lang_System_registerNatives(NATIVE_ARGS)
{
    registerNative("vigur/lang/System/printLn", "(Ljava/lang/String;)V", lib_vigur_lang_System_printLnString);
}

JCALL void lib_vigur_lang_System_printLnString(NATIVE_ARGS)
{
    Variable strVar = thread->m_currentFrame->localVariables[0];
    const Object* obj = heap->getObject(strVar.data);
    const FieldData* charArrRef = obj->getField("value", "[C", heap);
    const Array* charArr = heap->getArray(charArrRef->data[0].data);
    Platform::print((const char*)charArr->data, charArr->length);
    Platform::print("\n", 1);
}
