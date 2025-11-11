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

JCALL void lib_java_lang_Thread_registerNatives(const NativeArgs& args)
{
    registerNative("java/lang/Thread/currentThread", "()Ljava/lang/Thread;", lib_java_lang_Thread_currentThread);
    registerNative("java/lang/Thread/setPriority0", "(I)V", lib_java_lang_Thread_setPriority0);
    registerNative("java/lang/Thread/isAlive", "()Z", lib_java_lang_Thread_isAlive);
    registerNative("java/lang/Thread/start0", "()V", lib_java_lang_Thread_start0);
}

JCALL void lib_java_lang_Thread_currentThread(const NativeArgs& args)
{
    StackFrame* returnFrame = args.thread->getTopFrameNonNative();
    returnFrame->pushObject(args.thread->threadObject);
}

JCALL void lib_java_lang_Thread_setPriority0(const NativeArgs& args)
{
    const Object* threadObject = getThisObjectReference(args.thread, args.heap, args.vm);
    const StackFrame* currentFrame = args.thread->m_currentFrame;
    const Variable argument = currentFrame->localVariables[1];
    args.vm->checkType(argument, VariableType_INT, args.thread);

    FieldData* field = threadObject->getField("priority", "I", args.heap);
    field->data->data = argument.data;
}

JCALL void lib_java_lang_Thread_isAlive(const NativeArgs& args)
{
    const StackFrame* currentFrame = args.thread->m_currentFrame;
    const Variable var = currentFrame->localVariables[0];
    args.vm->checkType(var, VariableType_REFERENCE, args.thread);
    const u4 objectReference = var.data;
    const VMThread* vmThread =  args.vm->getVMThreadByObjectRef(objectReference);

    bool alive = false;
    if (vmThread != nullptr)
    {
        alive = vmThread->alive;
    }

    StackFrame* returnFrame = args.thread->getTopFrameNonNative();
    returnFrame->pushInt(alive? 1 : 0);
}

JCALL void lib_java_lang_Thread_start0(const NativeArgs& args)
{
    const Object* threadObject = getThisObjectReference(args.thread, args.heap,args.vm);
    const FieldData* runnableField = threadObject->getField("target", "Ljava/lang/Runnable;", args.heap);
    if (runnableField->data->data != 0)
    {
        args.thread->internalError("Running of Runnables, not implemented yet");
    } else
    {
        // TODO: Run the run method on a new thread
        // MethodInfo* methodInfo = threadObject->classInfo->findMethodWithNameAndDescriptor("run", "()V");
        // thread->pushStackFrameWithoutParams(threadObject->classInfo, methodInfo);
        // thread->m_currentFrame->localVariables[0] = thread->m_currentFrame->localVariables[0];
        // VM->executeLoop(thread);
    }
}

