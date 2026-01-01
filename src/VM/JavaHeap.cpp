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

#include "JavaHeap.h"

#include "physfs.h"
#include "CoreUtils.h"
#include "VM.h"

JavaHeap::JavaHeap()
{
    // We don't want the heap to return reference 0,
    // because this would be considered a null pointer
    objects.push_back(0);
}
vreference JavaHeap::createArray(ArrayType type, uint64_t size, std::string_view descriptor)
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
    Array* array = static_cast<Array*>(Platform::allocateMemory(sizeof(Array) + bytes * size, 0));
    array->length = size;
    array->type = ARRAY;
    array->arrayType = type;
    array->data = 0;
    array->descriptor = descriptor;
    u1* basePtr = static_cast<u1*>(static_cast<void*>(array));
    array->data = basePtr + sizeof(Array);
    for (int i = 0; i < size * bytes; ++i)
    {
        array->data[i] = 0;
    }

    objects.push_back(array);
    return castToU4<std::size_t>(objects.size()-1);
}

vreference JavaHeap::createObject(ClassInfo* classInfo)
{
    u2 fieldsCount = 0;
    for (u2 currentField = 0; currentField < classInfo->fieldsCount; ++currentField)
    {
        FieldInfo* fieldInfo = classInfo->fields[currentField];
        if (!fieldInfo->isStatic())
        {
            ++fieldsCount;
        }
    }

    void* objectMemory = Platform::allocateMemory(sizeof(Object) + sizeof(FieldData) * fieldsCount, 0);
    const auto object = new (objectMemory) Object;

    object->classInfo = classInfo;
    object->type = OBJECT;
    if (fieldsCount > 0)
    {
        u1* basePtr = static_cast<u1*>(objectMemory);
        const auto fields = static_cast<FieldData*>(static_cast<void*>(basePtr + sizeof(Object)));
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
            const VariableType type = fromDescriptor(descriptorText);
            data.dataSize = getCategoryFromVariableType(type);
            data.value.j = 0;
            data.type = type;
            object->fields[fieldsCount++] = data;
        }
    }

    // Check if we are not in java/lang/Object, because that class doesn't have a superClas
    if (classInfo->superClass != 0)
    {
        const std::string_view superClassName = classInfo->constantPool->getString(classInfo->constantPool->getClassInfo(classInfo->superClass)->nameIndex);
        ClassInfo* superClass = getClassByName(superClassName.data());
        const u4 superClassObject = createObject(superClass);
        object->superClassObject = superClassObject;
    }

    objects.push_back(object);
    return castToU4<std::size_t>(objects.size()-1);
}

// TODO: De-duplicate code from createObject
vreference JavaHeap::createClassObject(ClassInfo* classInfo, std::string_view name)
{
    const u4 existingClassObject = getClassObjectByName(name);
    if (existingClassObject != 0)
    {
        return existingClassObject;
    }

    u2 fieldsCount = 0;
    for (u2 currentField = 0; currentField < classClassInfo->fieldsCount; ++currentField)
    {
        const FieldInfo* fieldInfo = classClassInfo->fields[currentField];
        if (!fieldInfo->isStatic())
        {
            ++fieldsCount;
        }
    }

    void* objectMemory = Platform::allocateMemory(sizeof(ClassObject) + sizeof(FieldData) * fieldsCount, 0);
    const auto object = new (objectMemory) ClassObject();

    object->classInfo = classClassInfo;
    object->classClassInfo = classInfo;
    object->type = CLASSOBJECT;
    if (fieldsCount > 0)
    {
        u1* basePtr = static_cast<u1*>(objectMemory);
        const auto fields = static_cast<FieldData*>(static_cast<void*>(basePtr + sizeof(ClassObject)));
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
            const VariableType type = fromDescriptor(descriptorText);
            data.dataSize = getCategoryFromVariableType(type);
            data.value.j = 0;
            data.type = type;
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
            u4 superClassObject = createObject(superClass);
            object->superClassObject = superClassObject;
        }
    }

    objects.push_back(object);
    return static_cast<u4>(objects.size()-1);
}


vreference JavaHeap::createString(const char* utf8String) {
    const u4 existingString = getString(utf8String);
    if (existingString != 0) {
        return existingString;
    }

    const u4 strObjectId = createObject(getClassByName("java/lang/String"));
    const Object* strObject = getObject(strObjectId);
    const std::u16string_view u16String = u8ToU16String(utf8String);
    const u4 arrId = createArray(AT_CHAR, u16String.length(), "C"); // TODO: C?
    const Array* charArray = getArray(arrId);
    if (strlen(utf8String) > 0)
    {
        memcpy(charArray->data, u16String.data(), u16String.length() * sizeof(u2));
        free((void*)u16String.data());
    }

    strObject->fields[0].value.l = arrId;
    strObject->fields[0].type = VariableType_REFERENCE;

    return strObjectId;
}

const Object* JavaHeap::getObject(const vreference id) const
{
    if (id == 0)
    {
        // Nullpointer
        fprintf(stderr, "Error: Null pointer exception!\n");
        Platform::exitProgram(1);
    } else if (id >= objects.size()) {
        fprintf(stderr, "Error: Id out of bounds!\n");
        Platform::exitProgram(1);
    }
    return objects[id]->getObject();
}

Reference* JavaHeap::getReference(const vreference id) const
{
    if (id == 0)
    {
        // Nullpointer
        fprintf(stderr, "Error: Null pointer exception!\n");
        Platform::exitProgram(1);
    }
    return objects[id];
}

ClassObject* JavaHeap::getClassObject(vreference id) const
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

const Object* JavaHeap::getChildObject(const vreference id, ClassInfo* classInfo)
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

const Array* JavaHeap::getArray(const vreference id) const
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
    u4 utf8Size = strlen(utf8String);
    const std::u16string_view u16String = u8ToU16String(utf8String);

    for (uint32_t currentObj = 1; currentObj < objects.size(); ++currentObj)
    {
        const Reference* ref = objects[currentObj];
        if (ref->type == ReferenceType::OBJECT) {
            const Object* obj = static_cast<const Object*>(ref);
            if (obj->classInfo->getName() == std::string_view{"java/lang/String"})
            {
                const vreference charArrRef =  obj->fields[0].value.l;
                if (charArrRef == 0)
                {
                    printf("WARN: String contains null reference to characters");
                    continue;
                }
                const Array* arr = getArray(charArrRef);
                if (arr->length == 0 && u16String.length() == 0)
                {
                    return currentObj;
                }
                if (u16String.length() == arr->length)
                {
                    // printf("======= same size ...");
                    bool foundDifference = false;
                    for (u4 currentIndex = 0; currentIndex < arr->length; ++currentIndex)
                    {
                        if (u16String.data()[currentIndex] != ((u2*)arr->data)[currentIndex])
                        {
                            foundDifference = true;
                        }
                    }
                    if (!foundDifference)
                    {
                        // printf(" --> Found copy! of %s\n", utf8String);
                        return currentObj;
                    }
                }
            }
        }
    }
    if (utf8Size > 0)
    {
        free((void*)u16String.data());
    }
    return 0;
}

std::u16string_view JavaHeap::getStringContent(const Object* stringObject) const
{
    if (stringObject->classInfo->getName() != "java/lang/String")
    {
        fprintf(stderr, "Getting string content from non-string object");
        Platform::exitProgram(3);
    }

    const u4 arrayRefId = stringObject->fields[0].value.l;
    const Array* array = getArray(arrayRefId);

    char16_t* charArray = reinterpret_cast<char16_t*>(array->data);
    return std::u16string_view{charArray, array->length};
}

std::u16string_view JavaHeap::getStringContent(const vreference id) const
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
    for (vreference currentObj = 1u; currentObj < objects.size(); ++currentObj)
    {
        const Reference* ref = objects[currentObj];
        if (ref->type == ReferenceType::OBJECT) {
            const Object* obj = static_cast<const Object*>(ref);
            if (obj->classInfo->getName() == std::string_view{"java/lang/String"})
            {
                printf("|%d|", currentObj);
                vreference charArrRef =  obj->fields[0].value.l;
                const Array* arr = getArray(charArrRef);
                for (vreference currentIndex = 0; currentIndex < arr->length; ++currentIndex)
                {
                    u1 charDowncasted = ((u2*)arr->data)[currentIndex];
                    printf("%c", charDowncasted);
                }
                printf("|\n");
            }
        }
    }
}

FieldData* Object::getField(const char* name, const char* descriptor) const
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
        return VM::get()->getHeap()->getObject(superClassObject)->getField(name, descriptor);
    }
    fprintf(stderr, "Error: Field not resolved!");
    Platform::exitProgram(23);
    return nullptr;
}

const Object* Object::getObject(const u4 fieldIndex) const
{
    return VM::get()->getHeap()->getObject(fields[fieldIndex].value.l);
}

const vlong Object::getLong(u4 fieldIndex) const
{
    const FieldData fieldData = fields[fieldIndex];
    const vlong longValue = fieldData.value.j;
    return longValue;
}

