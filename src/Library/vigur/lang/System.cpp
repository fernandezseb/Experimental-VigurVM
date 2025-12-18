/*
 * Copyright (c) 2023-2025 Sebastiaan Fernandez.
 *
 * This file is part of VigurVM.
 *
 * VigurVM is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * VigurVM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with VigurVM.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "System.h"

JCALL void lib_vigur_lang_System_registerNatives(const NativeArgs& args)
{
    registerNative("vigur/lang/System/printLn", "(Ljava/lang/String;)V", lib_vigur_lang_System_printLnString);
}

JCALL void lib_vigur_lang_System_printLnString(const NativeArgs& args)
{
    const Variable strVar = args.thread->m_currentFrame->localVariables[0];
    const Object* obj = args.heap->getObject(strVar.data);
    const FieldData* charArrRef = obj->getField("value", "[C", args.heap);
    const Array* charArr = args.heap->getArray(charArrRef->data);
    // TODO: Convert UTF-16 to UTF-8
    Platform::print(reinterpret_cast<const char*>(charArr->data), charArr->length*2);
    Platform::print("\n", 1);
}
