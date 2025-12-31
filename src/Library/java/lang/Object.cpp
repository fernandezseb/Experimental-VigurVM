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
    registerNative("java/lang/Object/notifyAll", "()V", lib_java_lang_Object_notifyAll);
}

JCALL void lib_java_lang_Object_hashCode(const NativeArgs& args)
{
    const StackFrame* currentFrame = args.thread->m_currentFrame;
    const vdata var{VariableType_INT, static_cast<vint>(currentFrame->localVariables[0].getReference())};
    args.thread->returnVar(var);
}

JCALL void lib_java_lang_Object_getClass(const NativeArgs& args)
{
    // TODO: Implement getClass for arrays as well
    const StackFrame* currentFrame = args.thread->m_currentFrame;
    const Reference* reference = VM::get()->getHeap()->getReference(currentFrame->localVariables[0].getReference());

    if (reference->type == OBJECT)
    {
        const Object* object = VM::get()->getHeap()->getObject(currentFrame->localVariables[0].getReference());
        const u4 classObject = VM::get()->getHeap()->createClassObject(object->classInfo, object->classInfo->getName());
        args.thread->returnVar(vdata{VariableType_REFERENCE, classObject});
    } else if (reference->type == ARRAY)
    {
        const Array* array = VM::get()->getHeap()->getArray(currentFrame->localVariables[0].getReference());
        printf("brol");
        std::string name = std::string(array->descriptor);
        name.insert(0, "[");
        const u4 classObject = VM::get()->getHeap()->createClassObject(nullptr, name.c_str());
        args.thread->returnVar(vdata{VariableType_REFERENCE, classObject});
    } else
    {
        args.thread->internalError("Running getClass on unknown type of object", 12);
    }
}

JCALL void lib_java_lang_Object_clone(const NativeArgs& args)
{
    const Reference* reference = VM::get()->getHeap()->getReference(args.thread->m_currentFrame->localVariables[0].getReference());
    if (reference->type == ARRAY)
    {
        const Array* array = reference->getArray();
        const u4 cloneRef = VM::get()->getHeap()->createArray(array->arrayType, array->length, array->descriptor);
        const Array* clone = VM::get()->getHeap()->getArray(cloneRef);
        // TODO: Do the copy of the data
        u4* newArrayData = reinterpret_cast<u4*>(clone->data);
        u4* oldArrayData = reinterpret_cast<u4*>(array->data);
        for (u4 currentItemIndex = 0; currentItemIndex < array->length; ++currentItemIndex)
        {
            newArrayData[currentItemIndex] = oldArrayData[currentItemIndex];
        }
        args.thread->returnVar(vdata{VariableType_REFERENCE, cloneRef});
        printf("");
    } else if (reference->type == CLASSOBJECT || reference->type == OBJECT) {
        args.thread->internalError("Not implemented yet", 781);
    }
}

JCALL void lib_java_lang_Object_notifyAll(const NativeArgs& args)
{
    // TODO: Implement when multithreading logic is working
}
