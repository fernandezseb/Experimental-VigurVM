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
    // TODO: Implement getClass for arrays as well
    const StackFrame* currentFrame = args.thread->m_currentFrame;
    const Reference* reference = args.heap->getReference(currentFrame->localVariables[0].data);

    if (reference->type == OBJECT)
    {
        const Object* object = args.heap->getObject(currentFrame->localVariables[0].data);
        const u4 classObject = args.heap->createClassObject(object->classInfo, args.vm, object->classInfo->getName());
        args.thread->returnVar(Variable{VariableType_REFERENCE, classObject});
    } else if (reference->type == ARRAY)
    {
        const Array* array = args.heap->getArray(currentFrame->localVariables[0].data);
        printf("brol");
        std::string *name = new std::string(array->descriptor);
        name->insert(0, "[");
        const u4 classObject = args.heap->createClassObject(nullptr, args.vm, name->c_str());
        args.thread->returnVar(Variable{VariableType_REFERENCE, classObject});
    } else
    {
        args.thread->internalError("Running getClass on unknown type of object", 12);
    }
}

JCALL void lib_java_lang_Object_clone(const NativeArgs& args)
{
    const Reference* reference = args.heap->getReference(args.thread->m_currentFrame->localVariables[0].data);
    if (reference->type == ARRAY)
    {
        const Array* array = reference->getArray();
        const u4 cloneRef = args.heap->createArray(array->arrayType, array->length, array->descriptor);
        const Array* clone = args.heap->getArray(cloneRef);
        // TODO: Do the copy of the data
        u4* newArrayData = reinterpret_cast<u4*>(clone->data);
        u4* oldArrayData = reinterpret_cast<u4*>(array->data);
        for (u4 currentItemIndex = 0; currentItemIndex < array->length; ++currentItemIndex)
        {
            newArrayData[currentItemIndex] = oldArrayData[currentItemIndex];
        }
        args.thread->returnVar(Variable{VariableType_REFERENCE, cloneRef});
        printf("");
    } else if (reference->type == CLASSOBJECT || reference->type == OBJECT) {
        args.thread->internalError("Not implemented yet", 781);
    }
}
