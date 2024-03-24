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
    registerNative("java/lang/Object/clone", "()Ljava/lang/Object;", lib_java_lang_Object_clone);
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

JCALL void lib_java_lang_Object_clone(const NativeArgs& args)
{
    const Reference* reference = args.heap->getReference(args.thread->m_currentFrame->localVariables[0].data);
    if (reference->type == ARRAY)
    {
        const Array* array = reference->getArray();
        const u4 cloneRef = args.heap->createArray(array->arrayType, array->length);
        const Array* clone = args.heap->getArray(cloneRef);
        // TODO: Do the copy of the data
        args.thread->returnVar(Variable{VariableType_REFERENCE, cloneRef});
        printf("");
    } else if (reference->type == CLASSOBJECT || reference->type == OBJECT) {
        args.thread->internalError("Not implemented yet");
    }
}
