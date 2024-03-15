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

#include "Object.h"

JCALL void lib_java_lang_Object_registerNatives(const NativeArgs& args)
{
    registerNative("java/lang/Object/hashCode", "()I", lib_java_lang_Object_hashCode);
    registerNative("java/lang/Object/getClass", "()Ljava/lang/Class;", lib_java_lang_Object_getClass);
}

JCALL void lib_java_lang_Object_hashCode(const NativeArgs& args)
{
    const StackFrame* currentFrame = args.thread->m_currentFrame;
    const Variable var{VariableType_INT, currentFrame->localVariables[0].data};
    args.thread->returnVar(var);
}

JCALL void lib_java_lang_Object_getClass(const NativeArgs& args)
{
    const StackFrame* currentFrame = args.thread->m_currentFrame;
    const Object* object = args.heap->getObject(currentFrame->localVariables[0].data);
    const u4 classObject = args.heap->createClassObject(object->classInfo, args.vm, object->classInfo->getName());
    args.thread->returnVar(Variable{VariableType_REFERENCE, classObject});
}
