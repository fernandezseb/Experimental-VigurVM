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

#include "String.h"

[[nodiscard]] static const Object* getThisObjectReference(
    VMThread* thread,
    const JavaHeap* heap,
    const VM* VM)
{
    const StackFrame* currentFrame = thread->m_currentFrame;
    const Variable var = currentFrame->localVariables[0];
    VM->checkType(var, VariableType_REFERENCE, thread);
    return heap->getObject(var.data);
}

JCALL void lib_java_lang_String_intern(const NativeArgs& args)
{
    Variable var = args.thread->m_currentFrame->localVariables[0];
    // const Object* stringObject = getThisObjectReference(args.thread, args.heap, args.vm);
    // const u4 arrayReference = stringObject->fields[0].data->data;
    // const Array* charArray = args.heap->getArray(arrayReference);
    // const u4 stringRef = args.heap->createString(reinterpret_cast<const char*>(charArray->data), args.vm);
    // TODO: Ensure creation of string on the string pool
    printf("|String Interned with id: %d\n", var.data);
    args.thread->returnVar(Variable{VariableType_REFERENCE, var.data});
}
