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

JCALL void lib_java_lang_Thread_registerNatives(NATIVE_ARGS)
{
    registerNative("java/lang/Thread/currentThread", "()Ljava/lang/Thread;", lib_java_lang_Thread_currentThread);
}

JCALL void lib_java_lang_Thread_currentThread(NATIVE_ARGS)
{
    // TODO: Maybe check if an object was already created?
    ClassInfo* threadClass = VM->getClass("java/lang/Thread", thread);
    const u4 objectReference = heap->createObject(threadClass, VM);
    StackFrame* returnFrame = thread->getTopFrameNonNative();
    returnFrame->pushObject(objectReference);
}

