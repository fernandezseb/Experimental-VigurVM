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
#include "Data/Descriptor.h"

#include <algorithm>
#include <string>

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

[[nodiscard]] static const ClassObject* getThisClassObjectReference(
    VMThread* thread,
    const JavaHeap* heap,
    const VM* VM)
{
    const StackFrame* currentFrame = thread->m_currentFrame;
    const Variable var = currentFrame->localVariables[0];
    VM->checkType(var, VariableType_REFERENCE, thread);
    return heap->getClassObject(var.data);
}

JCALL void lib_java_lang_Class_registerNatives(const NativeArgs& args)
{
    registerNative("java/lang/Class/getPrimitiveClass", "(Ljava/lang/String;)Ljava/lang/Class;",
                   lib_java_lang_Class_getPrimitiveClass);
    registerNative("java/lang/Class/desiredAssertionStatus0", "(Ljava/lang/Class;)Z",
                   lib_java_lang_Class_desiredAssertionStatus0);
    registerNative("java/lang/Class/getName0", "()Ljava/lang/String;", lib_java_lang_Class_getName0);
    registerNative("java/lang/Class/forName0", "(Ljava/lang/String;ZLjava/lang/ClassLoader;Ljava/lang/Class;)Ljava/lang/Class;", lib_java_lang_Class_forName0);
    registerNative("java/lang/Class/getDeclaredFields0", "(Z)[Ljava/lang/reflect/Field;", lib_java_lang_Class_getDeclaredFields0);
    registerNative("java/lang/Class/isPrimitive", "()Z", lib_java_lang_Class_isPrimitive);
    registerNative("java/lang/Class/isAssignableFrom", "(Ljava/lang/Class;)Z", lib_java_lang_Class_isAssignableFrom);
    registerNative("java/lang/Class/isInterface", "()Z", lib_java_lang_Class_isInterface);
}

JCALL void lib_java_lang_Class_getPrimitiveClass(const NativeArgs& args)
{
    const Object* strObject = getThisObjectReference(args.thread, args.heap, args.vm);
    const std::string_view typeString = args.heap->getStringContent(strObject);
    u4 classRef = 0;
    if (typeString == "float")
    {
        ClassInfo* classInfo = args.vm->getClass("java/lang/Float", args.thread);
        classRef = args.heap->createClassObject(classInfo, args.vm, "java/lang/Float");
    }
    else if (typeString == "int")
    {
        ClassInfo* classInfo = args.vm->getClass("java/lang/Integer", args.thread);
        classRef = args.heap->createClassObject(classInfo, args.vm, "java/lang/Integer");
    }
    else if (typeString == "double")
    {
        ClassInfo* classInfo = args.vm->getClass("java/lang/Double", args.thread);
        classRef = args.heap->createClassObject(classInfo, args.vm, "java/lang/Double");
    }
    else if (typeString == "char")
    {
        ClassInfo* classInfo = args.vm->getClass("java/lang/Character", args.thread);
        classRef = args.heap->createClassObject(classInfo, args.vm, "java/lang/Character");
    }
    else if (typeString == "boolean")
    {
        ClassInfo* classInfo = args.vm->getClass("java/lang/Boolean", args.thread);
        classRef = args.heap->createClassObject(classInfo, args.vm, "java/lang/Boolean");
    }
    else
    {
        args.thread->internalError("Typestring not found");
    }
    args.thread->returnVar(Variable{VariableType_REFERENCE, classRef});
}

JCALL void lib_java_lang_Class_desiredAssertionStatus0(const NativeArgs& args)
{
    args.thread->returnVar(Variable{VariableType_INT, 0});
}

JCALL void lib_java_lang_Class_getName0(const NativeArgs& args)
{
    const ClassObject* classObject = getThisClassObjectReference(args.thread, args.heap, args.vm);
    const std::string_view name = classObject->name;
    const u4 stringObject = args.heap->createString(name.data(), args.vm);
    args.thread->returnVar(Variable{VariableType_REFERENCE, stringObject});
}

JCALL void lib_java_lang_Class_forName0(const NativeArgs& args)
{
    const StackFrame* currentFrame = args.thread->m_currentFrame;
    const Variable var = currentFrame->localVariables[0];
    args.vm->checkType(var, VariableType_REFERENCE, args.thread);
    const std::string_view className = args.heap->getStringContent(var.data);
    std::string canonicalClassName = std::string(className.data(), className.size());
    std::replace(canonicalClassName.begin(), canonicalClassName.end(),
        '.', '/');

    // TODO: Check initialize bool
    const Variable var2 = currentFrame->localVariables[1];
    args.vm->checkType(var2, VariableType_INT, args.thread);
    if (var2.data == 0u)
    {
        args.thread->internalError("Loading class without initialization is not implemente yet");
    }

    // TODO: Use custom classloader if defined
    const Variable var3 = currentFrame->localVariables[2];
    args.vm->checkType(var3, VariableType_REFERENCE, args.thread);
    if (var3.data != 0u)
    {
        args.thread->internalError("Use of custom classloader not implemented yet");
    }

    ClassInfo* classInfo = args.vm->getClass(canonicalClassName, args.thread);
    const u4 classObjectRef = args.heap->createClassObject(classInfo, args.vm, canonicalClassName);
    args.thread->returnVar(Variable{VariableType_REFERENCE, classObjectRef});
}

JCALL void lib_java_lang_Class_getDeclaredFields0(const NativeArgs& args)
{
    const ClassObject* classObject = getThisClassObjectReference(args.thread, args.heap, args.vm);
    const StackFrame* currentFrame = args.thread->m_currentFrame;
    // TODO: Use publicOnlyBooleanVar
    [[maybe_unused]] const Variable publicOnlyBooleanVar  = currentFrame->localVariables[1];
    const u4 arraySize = classObject->classClassInfo->fieldsCount;
    const u4 arrayObject =  args.heap->createArray(AT_REFERENCE, arraySize);

    const Array* fieldsArray = args.heap->getArray(arrayObject);

    ClassInfo* fieldClass = args.vm->getClass("java/lang/reflect/Field", args.thread);

    for (u4 currentField = 0; currentField < arraySize; ++currentField)
    {
        const u4 fieldObjectRef = args.heap->createObject(fieldClass, args.vm);
        const Object* fieldObject = args.heap->getObject(fieldObjectRef);
        const FieldInfo* fieldInfo = classObject->classClassInfo->fields[currentField];

        // Set the name field
        FieldData* nameField = fieldObject->getField("name", "Ljava/lang/String;", args.heap);
        std::string_view fieldName =  classObject->classClassInfo->constantPool->getString(fieldInfo->nameIndex);
        std::string_view fieldType = classObject->classClassInfo->constantPool->getString(fieldInfo->descriptorIndex);
        const u4 fieldNameStringObjectRef = args.heap->createString(fieldName.data(), args.vm);
        nameField->data->data = fieldNameStringObjectRef;

        // Set the slot field
        FieldData* slotField = fieldObject->getField("slot", "I", args.heap);
        slotField->data->data = currentField;

        // Set the class field
        FieldData* classField = fieldObject->getField("clazz", "Ljava/lang/Class;", args.heap);
        classField->data->data =  currentFrame->localVariables[0].data;

        // Set the modifiers field
        FieldData* modifiersField = fieldObject->getField("modifiers", "I", args.heap);
        modifiersField->data->data = fieldInfo->accessFlags;

        const FieldData* typeField = fieldObject->getField("type", "Ljava/lang/Class;", args.heap);
        const u4 classObjectRef = args.heap->createClassObject(nullptr, args.vm, getTypeAsString(fieldType));
        typeField->data->data = classObjectRef;


        u4* array = reinterpret_cast<u4*>(fieldsArray->data);
        array[currentField] = fieldObjectRef;
    }

    args.thread->returnVar(Variable{VariableType_REFERENCE, arrayObject});
}

JCALL void lib_java_lang_Class_isPrimitive(const NativeArgs& args)
{
    const ClassObject* classObject =  getThisClassObjectReference(args.thread, args.heap, args.vm);
    u4 result = 0;

    if (classObject->name == "int")
    {
        result = 1u;
    }

    if (classObject->name == "byte")
    {
        result = 1u;
    }

    if (classObject->name == "short")
    {
        result = 1u;
    }

    if (classObject->name == "float")
    {
        result = 1u;
    }

    if (classObject->name == "long")
    {
        result = 1u;
    }

    if (classObject->name == "double")
    {
        result = 1u;
    }

    if (classObject->name == "char")
    {
        result = 1u;
    }

    if (classObject->name == "boolean")
    {
        result = 1u;
    }

    args.thread->returnVar(Variable{VariableType_INT, result});
}

JCALL void lib_java_lang_Class_isAssignableFrom(const NativeArgs& args)
{
    const ClassObject* thisClassObject =  getThisClassObjectReference(args.thread, args.heap, args.vm);

    const StackFrame* currentFrame = args.thread->m_currentFrame;
    const Variable var = currentFrame->localVariables[1];
    args.vm->checkType(var, VariableType_REFERENCE, args.thread);
    const ClassObject* clsClassObject =  args.heap->getClassObject(var.data);

    // TODO: Add type conversion checking
    const u4 result = args.vm->isSubclass(args.thread, thisClassObject->classClassInfo, clsClassObject->classClassInfo);

    args.thread->returnVar(Variable{VariableType_INT, result});
}

JCALL void lib_java_lang_Class_isInterface(const NativeArgs& args)
{
    const ClassObject* classObject = args.heap->getClassObject(args.thread->m_currentFrame->localVariables[0].data);
    u4 result = 0;
    if (classObject->classClassInfo->isInterface())
    {
        result = 1;
    }
    args.thread->returnVar(Variable{VariableType_INT, result});
}
