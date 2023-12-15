#include "JavaHeap.h"


JavaHeap::JavaHeap()
{
    // We don't want the heap to return reference 0,
    // because this would be considered a null pointer
    objects.push_back(0);
}

uint32_t JavaHeap::createArray(ArrayType type, uint64_t size)
{
    Array* array = (Array*) Platform::allocateMemory(sizeof(Array), 0);
    array->length = size;
    array->type = ARRAY;
    array->arrayType = AT_REFERENCE;
    array->data = (u1*) Platform::allocateMemory(sizeof(uint32_t) * size, 0);
    for (int i = 0; i < size; ++i)
    {
        array->data[i] = 0;
    }
    objects.push_back(array);
    return objects.size()-1;
}

uint32_t JavaHeap::createObject(ClassInfo* class_info)
{
    Object* object = (Object*) Platform::allocateMemory(sizeof(Array), 0);


    u2 fieldsCount = 0;

    for (u2 currentField = 0; currentField < class_info->fieldsCount; ++currentField)
    {
        FieldInfo* fieldInfo = class_info->fields[currentField];
        if (!fieldInfo->isStatic())
        {
            ++fieldsCount;
        }
    }

    object->classInfo = class_info;
    object->type = OBJECT;
    object->fields = 0;
    if (fieldsCount > 0)
    {
        object->fields = (FieldData*) Platform::allocateMemory(sizeof(FieldData) * fieldsCount, 0);
    }
    object->fieldsCount = fieldsCount;

    fieldsCount = 0;
    for (u2 currentField = 0; currentField < class_info->fieldsCount; ++currentField)
    {
        FieldInfo* fieldInfo = class_info->fields[currentField];
        if (!fieldInfo->isStatic())
        {
            Variable var = {};
            var.data = 0;
            var.type = VariableType_UNDEFINED;
            FieldData data = {};
            data.descriptorIndex = fieldInfo->descriptorIndex;
            data.nameIndex = fieldInfo->nameIndex;
            data.data = var;
            object->fields[fieldsCount++] = data;
        }
    }

    // TODO: Initialize the fields resursive

    objects.push_back(object);
    return objects.size()-1;
}

Object* JavaHeap::getObject(uint32_t id)
{
    if (id == 0)
    {
        // Nullpointer
        fprintf(stderr, "Error: Null pointer exception!");
        Platform::exitProgram(-1);
    }
    Reference* ref = objects[id];
    if (ref->type == OBJECT)
    {
        return (Object*) objects[id];
    } else
    {
        fprintf(stderr, "Error: Array instead of Object");
        Platform::exitProgram(-22);
    }
}

Array* JavaHeap::getArray(uint32_t id)
{
    if (id == 0)
    {
        // Nullpointer
        fprintf(stderr, "Error: Null pointer exception!");
        Platform::exitProgram(-1);
    }
    Reference* ref = objects[id];
    if (ref->type == ARRAY)
    {
        return (Array*) objects[id];
    } else
    {
        fprintf(stderr, "Error: Array instead of Object");
        Platform::exitProgram(-22);
    }
}

void JavaHeap::addClassInfo(ClassInfo* classInfo)
{
    methodArea.classes.add(classInfo);
}

ClassInfo* JavaHeap::getClassByName(const char* className)
{
    for (size_t currentClass = 0; currentClass < methodArea.classes.getSize() ; currentClass++)
    {
        ClassInfo* classInfo = methodArea.classes.get(currentClass);
        if (strcmp(classInfo->getName(), className) == 0)
        {
            return classInfo;
        }
    }
    return 0;
}
