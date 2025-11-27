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
    registerNative("java/lang/Class/getDeclaredConstructors0", "(Z)[Ljava/lang/reflect/Constructor;", lib_java_lang_Class_getDeclaredConstructors0);
    registerNative("java/lang/Class/getModifiers", "()I", lib_java_lang_Class_getModifiers);
    registerNative("java/lang/Class/getSuperclass", "()Ljava/lang/Class;", lib_java_lang_Class_getSuperClass);
    registerNative("java/lang/Class/getInterfaces0", "()[Ljava/lang/Class;", lib_java_lang_Class_getInterfaces0);
}

JCALL void lib_java_lang_Class_getPrimitiveClass(const NativeArgs& args)
{
    const Object* strObject = getThisObjectReference(args.thread, args.heap, args.vm);
    const J16String typeString = args.heap->getStringContent(strObject);
    std::u16string u16str = std::u16string((char16_t*)typeString.chars, typeString.length);
    u4 classRef = 0;
    if (u16str == u"float")
    {
        ClassInfo* classInfo = args.vm->getClass("java/lang/Float", args.thread);
        classRef = args.heap->createClassObject(classInfo, args.vm, "java/lang/Float");
    }
    else if (u16str == u"int")
    {
        ClassInfo* classInfo = args.vm->getClass("java/lang/Integer", args.thread);
        classRef = args.heap->createClassObject(classInfo, args.vm, "java/lang/Integer");
    }
    else if (u16str == u"double")
    {
        ClassInfo* classInfo = args.vm->getClass("java/lang/Double", args.thread);
        classRef = args.heap->createClassObject(classInfo, args.vm, "java/lang/Double");
    }
    else if (u16str == u"char")
    {
        ClassInfo* classInfo = args.vm->getClass("java/lang/Character", args.thread);
        classRef = args.heap->createClassObject(classInfo, args.vm, "java/lang/Character");
    }
    else if (u16str == u"boolean")
    {
        ClassInfo* classInfo = args.vm->getClass("java/lang/Boolean", args.thread);
        classRef = args.heap->createClassObject(classInfo, args.vm, "java/lang/Boolean");
    }
    else if (u16str == u"long")
    {
        ClassInfo* classInfo = args.vm->getClass("java/lang/Long", args.thread);
        classRef = args.heap->createClassObject(classInfo, args.vm, "java/lang/Long");
    }
    else
    {
        args.thread->internalError("Typestring not found", 45);
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
    // TODO: Allocate this string on the memory collected heap
    const J16String className = args.heap->getStringContent(var.data);
    std::u16string *canonicalClassName = new std::u16string((char16_t*)className.chars, className.length);
    // std::string *canonicalClassName = new std::string(className.data(), className.size());
    std::replace(canonicalClassName->begin(), canonicalClassName->end(),
        '.', '/');

    // TODO: Check initialize bool
    const Variable var2 = currentFrame->localVariables[1];
    args.vm->checkType(var2, VariableType_INT, args.thread);
    if (var2.data == 0u)
    {
        args.thread->internalError("Loading class without initialization is not implemente yet", 44);
    }

    // TODO: Use custom classloader if defined
    const Variable var3 = currentFrame->localVariables[2];
    args.vm->checkType(var3, VariableType_REFERENCE, args.thread);
    if (var3.data != 0u)
    {
        args.thread->internalError("Use of custom classloader not implemented yet", -3);
    }

    const char* utf8ClassName = J16StringToUtf8String(J16String((u2*)canonicalClassName->data(), canonicalClassName->length()));
    ClassInfo* classInfo = args.vm->getClass(utf8ClassName, args.thread);
    const u4 classObjectRef = args.heap->createClassObject(classInfo, args.vm, utf8ClassName);
    free((void*)utf8ClassName);
    args.thread->returnVar(Variable{VariableType_REFERENCE, classObjectRef});
}

JCALL void lib_java_lang_Class_getDeclaredFields0(const NativeArgs& args)
{
    const ClassObject* classObject = getThisClassObjectReference(args.thread, args.heap, args.vm);
    const StackFrame* currentFrame = args.thread->m_currentFrame;
    // TODO: Use publicOnlyBooleanVar
    [[maybe_unused]] const Variable publicOnlyBooleanVar  = currentFrame->localVariables[1];
    const u4 arraySize = classObject->classClassInfo->fieldsCount;
    const u4 arrayObject =  args.heap->createArray(AT_REFERENCE, arraySize, "Ljava/lang/reflect/Field;");

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

JCALL void lib_java_lang_Class_getDeclaredConstructors0(const NativeArgs& args)
{
    const ClassObject* classObject = getThisClassObjectReference(args.thread, args.heap, args.vm);
    const StackFrame* currentFrame = args.thread->m_currentFrame;
    // TODO: Use publicOnlyBooleanVar
    [[maybe_unused]] const Variable publicOnlyBooleanVar  = currentFrame->localVariables[1];
    const u4 arraySize = classObject->classClassInfo->methods.size();
    const u4 arrayObject =  args.heap->createArray(AT_REFERENCE, 1, "Ljava/lang/reflect/Constructor,");

    const Array* constructorsArray = args.heap->getArray(arrayObject);

    ClassInfo* constructorClass = args.vm->getClass("java/lang/reflect/Constructor", args.thread);

    for (u4 currentMethod = 0; currentMethod < arraySize; ++currentMethod)
    {
        const u4 constructorObjectRef = args.heap->createObject(constructorClass, args.vm);
        const Object* constructorObject = args.heap->getObject(constructorObjectRef);
        const MethodInfo* methodInfo = classObject->classClassInfo->methods[currentMethod];

        // Set the slot field
        FieldData* slotField = constructorObject->getField("slot", "I", args.heap);
        slotField->data->data = currentMethod;

        // Set the class field
        FieldData* classField = constructorObject->getField("clazz", "Ljava/lang/Class;", args.heap);
        classField->data->data =  currentFrame->localVariables[0].data;

        // Set the modifiers field
        FieldData* modifiersField = constructorObject->getField("modifiers", "I", args.heap);
        modifiersField->data->data = methodInfo->accessFlags;

        // Set the parameterTypes field
        FieldData* parameterTypes = constructorObject->getField("parameterTypes", "[Ljava/lang/Class;", args.heap);
        // TODO: Not sure about the descriptor in the array
        const u4 argsArray = args.heap->createArray(AT_REFERENCE, 0, "Ljava/lang/Class;");
        parameterTypes->data->data = argsArray;

        u4* array = reinterpret_cast<u4*>(constructorsArray->data);
        array[currentMethod] = constructorObjectRef;
    }

    args.thread->returnVar(Variable{VariableType_REFERENCE, arrayObject});
    printf("");
}

JCALL void lib_java_lang_Class_getModifiers(const NativeArgs& args)
{
    const ClassObject* classObject = getThisClassObjectReference(args.thread, args.heap, args.vm);
    args.thread->returnVar(Variable{VariableType_INT, classObject->classClassInfo->accessFlags});
    printf("");
}

JCALL void lib_java_lang_Class_getSuperClass(const NativeArgs& args)
{
    const ClassObject* classObject = getThisClassObjectReference(args.thread, args.heap, args.vm);
    if (classObject->classClassInfo->superClass != 0)
    {
        const CPClassInfo* classInfoConstant = classObject->classClassInfo->constantPool->getClassInfo(classObject->classClassInfo->superClass);
        const std::string_view className =  classObject->classClassInfo->constantPool->getString(classInfoConstant->nameIndex);
        ClassInfo* classInfo =  args.vm->getClass(className, args.thread);
        const u4 superClassObject = args.heap->createClassObject(classInfo,args.vm,  className);
        args.thread->returnVar(Variable{VariableType_REFERENCE, superClassObject});
    } else
    {
        args.thread->returnVar(Variable{VariableType_REFERENCE, 0});
    }
    printf("");
}

JCALL void lib_java_lang_Class_getInterfaces0(const NativeArgs& args)
{
    const ClassObject* classObject = getThisClassObjectReference(args.thread, args.heap, args.vm);
    std::span<uint16_t> interfaces = classObject->classClassInfo->interfaces;

    const u4 newArrayRef = args.heap->createArray(ArrayType::AT_REFERENCE, interfaces.size(), "Ljava/lang/Class;");
    const Array* newArray = args.heap->getArray(newArrayRef);
    u4* arrayData = reinterpret_cast<u4*>(newArray->data);

    for (uint16_t currentInterface = 0; currentInterface < interfaces.size(); ++currentInterface)
    {
        CPClassInfo* intefaceInfo = classObject->classClassInfo->constantPool->getClassInfo(interfaces[currentInterface]);
        std::string_view interfaceName = classObject->classClassInfo->constantPool->getString(intefaceInfo->nameIndex);
        ClassInfo* classInfo =  args.vm->getClass(interfaceName, args.thread);
        const u4 interfaceObject = args.heap->createClassObject(classInfo,args.vm,  interfaceName);
        arrayData[currentInterface] = interfaceObject;
        printf("brol");
    }
    printf("");
    args.thread->returnVar(Variable{VariableType_REFERENCE, newArrayRef});
}
