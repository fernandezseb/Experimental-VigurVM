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

#include "JavaHeap.h"

#include "Util.h"
#include "VM.h"

JavaHeap::JavaHeap()
{
    // We don't want the heap to return reference 0,
    // because this would be considered a null pointer
    objects.push_back(0);
}

u4 JavaHeap::createArray(ArrayType type, uint64_t size, std::string_view descriptor)
{
    Array* array = static_cast<Array*>(Platform::allocateMemory(sizeof(Array), 0));
    array->length = size;
    array->type = ARRAY;
    array->arrayType = type;
    array->data = 0;
    array->descriptor = descriptor;
    if (size > 0 )
    {
        u1 bytes = 4;
        if (type == AT_CHAR)
        {
            bytes = 2;
        }
        else if (type == AT_LONG || type == AT_DOUBLE)
        {
            bytes = 8;
        }
        array->data = (u1*) Platform::allocateMemory(bytes * size, 0);
        for (int i = 0; i < size; ++i)
        {
            array->data[i] = 0;
        }
    }

    objects.push_back(array);
    return castToU4<std::size_t>(objects.size()-1);
}

u4 JavaHeap::createObject(ClassInfo* classInfo, VM* VM)
{
    void* objectMemory = Platform::allocateMemory(sizeof(Object), 0);
    const auto object = new (objectMemory) Object;
    u2 fieldsCount = 0;
    for (u2 currentField = 0; currentField < classInfo->fieldsCount; ++currentField)
    {
        FieldInfo* fieldInfo = classInfo->fields[currentField];
        if (!fieldInfo->isStatic())
        {
            ++fieldsCount;
        }
    }

    object->classInfo = classInfo;
    object->type = OBJECT;
    if (fieldsCount > 0)
    {
        const auto fields = static_cast<FieldData*>(Platform::allocateMemory(sizeof(FieldData) * fieldsCount, 0));
        object->fields = std::span{fields,fieldsCount};
    }
    object->fieldsCount = fieldsCount;
    object->superClassObject = 0;

    fieldsCount = 0;
    for (u2 currentField = 0; currentField < classInfo->fieldsCount; ++currentField)
    {
        const FieldInfo* fieldInfo = classInfo->fields[currentField];
        if (!fieldInfo->isStatic())
        {
            FieldData data = {};
            data.descriptorIndex = fieldInfo->descriptorIndex;
            data.nameIndex = fieldInfo->nameIndex;
            const std::string_view descriptorText = classInfo->constantPool->getString(fieldInfo->descriptorIndex);
            std::vector<Variable> vars = VM->createVariableForDescriptor(descriptorText);
            Variable* varsAllocated = (Variable*) Platform::allocateMemory(sizeof(Variable) * vars.size(), 0);
            for (u1 currentVar = 0; currentVar < vars.size(); ++currentVar)
            {
                varsAllocated[currentVar] = vars[currentVar];
            }
            data.dataSize = castToU1<std::size_t>(vars.size());
            data.data = varsAllocated;
            object->fields[fieldsCount++] = data;
        }
    }

    // Check if we are not in java/lang/Object, because that class doesn't have a superClas
    if (classInfo->superClass != 0)
    {
        const std::string_view superClassName = classInfo->constantPool->getString(classInfo->constantPool->getClassInfo(classInfo->superClass)->nameIndex);
        ClassInfo* superClass = getClassByName(superClassName.data());
        const u4 superClassObject = createObject(superClass, VM);
        object->superClassObject = superClassObject;
    }

    objects.push_back(object);
    return castToU4<std::size_t>(objects.size()-1);
}

// TODO: De-duplicate code from createObject
u4 JavaHeap::createClassObject(ClassInfo* classInfo, VM* VM, std::string_view name)
{
    const u4 existingClassObject = getClassObjectByName(name);
    if (existingClassObject != 0)
    {
        return existingClassObject;
    }

    void* objectMemory = Platform::allocateMemory(sizeof(ClassObject), 0);
    const auto object = new (objectMemory) ClassObject();
    u2 fieldsCount = 0;
    for (u2 currentField = 0; currentField < classClassInfo->fieldsCount; ++currentField)
    {
        const FieldInfo* fieldInfo = classClassInfo->fields[currentField];
        if (!fieldInfo->isStatic())
        {
            ++fieldsCount;
        }
    }

    object->classInfo = classClassInfo;
    object->classClassInfo = classInfo;
    object->type = CLASSOBJECT;
    if (fieldsCount > 0)
    {
        const auto fields = static_cast<FieldData*>(Platform::allocateMemory(sizeof(FieldData) * fieldsCount, 0));
        object->fields = std::span(fields, fieldsCount);
    }
    object->fieldsCount = fieldsCount;
    object->superClassObject = 0;
    object->name = name;

    fieldsCount = 0;
    for (u2 currentField = 0; currentField < classClassInfo->fieldsCount; ++currentField)
    {
        const FieldInfo* fieldInfo = classClassInfo->fields[currentField];
        if (!fieldInfo->isStatic())
        {
            FieldData data = {};
            data.descriptorIndex = fieldInfo->descriptorIndex;
            data.nameIndex = fieldInfo->nameIndex;
            const std::string_view descriptorText = classClassInfo->constantPool->getString(fieldInfo->descriptorIndex);
            std::vector<Variable> vars = VM->createVariableForDescriptor(descriptorText);
            Variable* varsAllocated = (Variable*) Platform::allocateMemory(sizeof(Variable) * vars.size(), 0);
            for (u1 currentVar = 0; currentVar < vars.size(); ++currentVar)
            {
                varsAllocated[currentVar] = vars[currentVar];
            }
            data.dataSize = castToU1<std::size_t>(vars.size());
            data.data = varsAllocated;
            object->fields[fieldsCount++] = data;
        }
    }

    // Check if we are not in java/lang/Object, because that class doesn't have a superClas
    if (classClassInfo->superClass != 0)
    {
        const std::string_view superClassName = classClassInfo->constantPool->getString(classClassInfo->constantPool->getClassInfo(classClassInfo->superClass)->nameIndex);
        if (superClassName != "java/lang/Object")
        {
            // TODO: Load the class if needed
            ClassInfo* superClass = getClassByName(superClassName.data());
            u4 superClassObject = createObject(superClass, VM);
            object->superClassObject = superClassObject;
        }
    }

    objects.push_back(object);
    return static_cast<u4>(objects.size()-1);
}


u4 JavaHeap::createString(const char* utf8String, VM* VM) {
    const u4 existingString = getString(utf8String);
    if (existingString != 0) {
        return existingString;
    }

    const u4 strObjectId = createObject(getClassByName("java/lang/String"), VM);
    const Object* strObject = getObject(strObjectId);

    const u4 arrId = createArray(AT_CHAR, strlen(utf8String), "C"); // TODO: C?
    const Array* charArray = getArray(arrId);
    // TODO: Correctly convert utf-8 modified to utf16
    for (u4 currentIndex = 0; currentIndex < strlen(utf8String); currentIndex++) {
        ((u2*)(charArray->data))[currentIndex] = utf8String[currentIndex];
    }

    const Variable var{VariableType_REFERENCE, arrId};
    strObject->fields[0].data[0] = var;

    return strObjectId;
}

const Object* JavaHeap::getObject(const uint32_t id) const
{
    if (id == 0)
    {
        // Nullpointer
        fprintf(stderr, "Error: Null pointer exception!\n");
        Platform::exitProgram(1);
    }
    return objects[id]->getObject();
}

Reference* JavaHeap::getReference(u4 id) const
{
    if (id == 0)
    {
        // Nullpointer
        fprintf(stderr, "Error: Null pointer exception!\n");
        Platform::exitProgram(1);
    }
    return objects[id];
}

ClassObject* JavaHeap::getClassObject(uint32_t id) const
{
    if (id == 0)
    {
        // Nullpointer
        fprintf(stderr, "Error: Null pointer exception!\n");
        Platform::exitProgram(1);
    }
    Reference* ref = objects[id];
    if (ref->type == CLASSOBJECT)
    {
        return static_cast<ClassObject*>(objects[id]);
    } else
    {
        fprintf(stderr, "Error: Array or Object instead of ClassObject\n");
        Platform::exitProgram(22);
    }
    return nullptr;
}

const Object* JavaHeap::getChildObject(const uint32_t id, ClassInfo* classInfo)
{
    const Object* o = getObject(id);
    if (o == nullptr || o->classInfo->getName() != classInfo->getName())
    {
        if (o != nullptr && o->superClassObject != 0)
        {
            return getChildObject(o->superClassObject, classInfo);
        }

        fprintf(stderr, "Error: Object not found at reference with class: %s\n", classInfo->getName().data());
        Platform::exitProgram(22);
    }
    return o;
}

const Array* JavaHeap::getArray(const uint32_t id) const
{
    if (id == 0)
    {
        // Nullpointer
        fprintf(stderr, "Error: Null pointer exception!");
        Platform::exitProgram(1);
    }
    return objects[id]->getArray();
}

u4 JavaHeap::getString(const char* utf8String) const
{
    u4 size = strlen(utf8String);
    u2* utf16Chars = new u2[size];

    // TODO: Correctly convert utf-8 modified to utf16
    for (u4 currentIndex = 0; currentIndex < strlen(utf8String); currentIndex++) {
        utf16Chars[currentIndex] = utf8String[currentIndex];
    }

    for (uint32_t currentObj = 1; currentObj < objects.size(); ++currentObj)
    {
        const Reference* ref = objects[currentObj];
        if (ref->type == ReferenceType::OBJECT) {
            const Object* obj = static_cast<const Object*>(ref);
            if (obj->classInfo->getName() == std::string_view{"java/lang/String"})
            {
                Variable charArrRef =  obj->fields[0].data[0];
                const Array* arr = getArray(charArrRef.data);
                if (arr->length == 0)
                {
                    if (strlen(utf8String) == 0)
                    {
                        return currentObj;
                    }
                } else {
                    if (strlen(utf8String) == arr->length)
                    {
                        // printf("======= same size ...");
                        bool foundDifference = false;
                        for (u4 currentIndex = 0; currentIndex < arr->length; ++currentIndex)
                        {
                            if (utf16Chars[currentIndex] != ((u2*)arr->data)[currentIndex])
                            {
                                foundDifference = true;
                            }
                        }
                        if (foundDifference)
                        {
                            continue;
                        } else
                        {
                            printf(" --> Found copy! of %s\n", utf8String);
                            return currentObj;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

std::string_view JavaHeap::getStringContent(const Object* stringObject) const
{
    if (stringObject->classInfo->getName() != "java/lang/String")
    {
        fprintf(stderr, "Getting string content from non-string object");
        Platform::exitProgram(3);
    }

    const u4 arrayRefId = stringObject->fields[0].data->data;
    const Array* array = getArray(arrayRefId);
    // TODO: Fix interpretation of this UTF16 string
    // std::string_view sv = std::string_view{reinterpret_cast<char*>(array->data), array->length*2};
    char *str = new char[array->length+1];

    const u2* charArray = reinterpret_cast<const u2*>(array->data);

    u4 new_current_length = 0;
    for (u4 currentChar = 0; currentChar < array->length; currentChar++)
    {
        str[currentChar] = charArray[currentChar];
    }

    std::string_view sv = std::string_view{str, array->length};
    return sv;
}

std::string_view JavaHeap::getStringContent(const u4 id) const
{
    const Object* stringObject = getObject(id);
    return getStringContent(stringObject);
}

u4 JavaHeap::getClassObjectByName(std::string_view name) const
{
    for (uint32_t currentObj = 1; currentObj < objects.size(); ++currentObj)
    {
        const Reference* ref = objects[currentObj];
        if (ref->type == ReferenceType::CLASSOBJECT) {
            const auto* obj = static_cast<const ClassObject*>(ref);
            if (obj->name == name) {
                return currentObj;
            }
        }
    }
    return 0;
}

void JavaHeap::addClassInfo(ClassInfo* classInfo)
{
    methodArea.classes.add(classInfo);
}

ClassInfo* JavaHeap::getClassByName(std::string_view className) const
{
    for (size_t currentClass = 0; currentClass < methodArea.classes.getSize() ; currentClass++)
    {
        ClassInfo* classInfo = methodArea.classes.get(currentClass);
        if (classInfo->getName() == className)
        {
            return classInfo;
        }
    }
    return nullptr;
}

void JavaHeap::printStringPool()
{
    for (uint32_t currentObj = 1; currentObj < objects.size(); ++currentObj)
    {
        const Reference* ref = objects[currentObj];
        if (ref->type == ReferenceType::OBJECT) {
            const Object* obj = static_cast<const Object*>(ref);
            if (obj->classInfo->getName() == std::string_view{"java/lang/String"})
            {
                printf("|%d|", currentObj);
                Variable charArrRef =  obj->fields[0].data[0];
                const Array* arr = getArray(charArrRef.data);
                for (u4 currentIndex = 0; currentIndex < arr->length; ++currentIndex)
                {
                    u1 charDowncasted = ((u2*)arr->data)[currentIndex];
                    printf("%c", charDowncasted);
                }
                printf("|\n");
            }
        }
    }
}

FieldData* Object::getField(const char* name, const char* descriptor, JavaHeap* heap) const
{
    bool found = false;
    for (u2 currentField = 0; currentField < fieldsCount; ++currentField)
    {
        const std::string_view targetName = classInfo->constantPool->getString(fields[currentField].nameIndex);
        const std::string_view targetDescriptor = classInfo->constantPool->getString(fields[currentField].descriptorIndex);

        if (name == targetName && targetDescriptor == descriptor)
        {
            found = true;
            return &fields[currentField];
        }
    }
    if (!found && superClassObject != 0)
    {
        return heap->getObject(superClassObject)->getField(name, descriptor, heap);
    }
    fprintf(stderr, "Error: Field not resolved!");
    Platform::exitProgram(23);
    return nullptr;
}
