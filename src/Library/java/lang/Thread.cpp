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

#include "Thread.h"

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

JCALL void lib_java_lang_Thread_registerNatives(NATIVE_ARGS)
{
    registerNative("java/lang/Thread/currentThread", "()Ljava/lang/Thread;", lib_java_lang_Thread_currentThread);
    registerNative("java/lang/Thread/setPriority0", "(I)V", lib_java_lang_Thread_setPriority0);
}

JCALL void lib_java_lang_Thread_currentThread(NATIVE_ARGS)
{
    StackFrame* returnFrame = thread->getTopFrameNonNative();
    returnFrame->pushObject(thread->threadObject);
}

JCALL void lib_java_lang_Thread_setPriority0(NATIVE_ARGS)
{
    const Object* threadObject = getThisObjectReference(thread, heap, VM);


    const StackFrame* currentFrame = thread->m_currentFrame;
    const Variable argument = currentFrame->localVariables[1];
    VM->checkType(argument, VariableType_INT, thread);

    FieldData* field = threadObject->getField("priority", "I", heap);
    field->data->data = argument.data;
}

