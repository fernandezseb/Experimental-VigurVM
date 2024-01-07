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

#include "Class.h"

JCALL void lib_java_lang_Class_registerNatives(NATIVE_ARGS)
{
    registerNative("java/lang/Class/getPrimitiveClass", "(Ljava/lang/String;)Ljava/lang/Class;", lib_java_lang_Class_getPrimitiveClass);
}

JCALL void lib_java_lang_Class_getPrimitiveClass(NATIVE_ARGS)
{
    const StackFrame* currentFrame = thread->m_currentFrame;
    const Variable var = currentFrame->localVariables[0];
    VM->checkType(var, VariableType_REFERENCE, thread);
    Object* strObject = heap->getObject(var.data);
    u4 arrayRefId = strObject->fields[0].data->data;
    Array* array = heap->getArray(arrayRefId);
    auto typeString = std::string_view{reinterpret_cast<char*>(array->data), array->length};
    u4 classRef = 0;
    if (typeString == "float")
    {
        ClassInfo* classInfo = VM->getClass("java/lang/Float", thread);
        classRef = heap->createClassObject(classInfo, VM, "java/lang/Float");
    } else if (typeString == "int")
    {
        ClassInfo* classInfo = VM->getClass("java/lang/Integer", thread);
        classRef = heap->createClassObject(classInfo, VM, "java/lang/Integer");
    } else if (typeString == "double")
    {
        ClassInfo* classInfo = VM->getClass("java/lang/Double", thread);
        classRef = heap->createClassObject(classInfo, VM, "java/lang/Double");
    } else if (typeString == "char")
    {
        ClassInfo* classInfo = VM->getClass("java/lang/Character", thread);
        classRef = heap->createClassObject(classInfo, VM, "java/lang/Character");
    } else if (typeString == "boolean")
    {
        ClassInfo* classInfo = VM->getClass("java/lang/Boolean", thread);
        classRef = heap->createClassObject(classInfo, VM, "java/lang/Boolean");
    }
    else
    {
        thread->internalError("Typestring not found");
    }
    thread->returnVar(Variable{VariableType_REFERENCE, classRef});
}
