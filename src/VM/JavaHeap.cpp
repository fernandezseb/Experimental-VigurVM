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
    array->arrayType = type;
    array->data = 0;
    if (size > 0 )
    {
        u1 bytes = 4;
        if (type == AT_CHAR)
        {
            bytes = 1;
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
    return objects.size()-1;
}

uint32_t JavaHeap::createObject(ClassInfo* class_info)
{
    Object* object = (Object*) Platform::allocateMemory(sizeof(Object), 0);
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
    object->superClassObject = 0;

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


    const char* superClassName = class_info->constantPool->getString(class_info->constantPool->getClassInfo(class_info->superClass)->nameIndex);
    if (strcmp(superClassName, "java/lang/Object") != 0)
    {
        ClassInfo* superClass = getClassByName(superClassName);
        u4 superClassObject = createObject(superClass);
        object->superClassObject = superClassObject;
    }

    objects.push_back(object);
    return objects.size()-1;
}

Object* JavaHeap::getObject(uint32_t id)
{
    if (id == 0)
    {
        // Nullpointer
        fprintf(stderr, "Error: Null pointer exception!\n");
        Platform::exitProgram(-1);
    }
    Reference* ref = objects[id];
    if (ref->type == OBJECT)
    {
        return (Object*) objects[id];
    } else
    {
        fprintf(stderr, "Error: Array instead of Object\n");
        Platform::exitProgram(-22);
    }
}

Object* JavaHeap::getChildObject(uint32_t id, ClassInfo* classInfo)
{
    Object* o = getObject(id);
    if (o == 0 || strcmp(o->classInfo->getName(), classInfo->getName()) != 0)
    {
        if (o->superClassObject != 0)
        {
            return getChildObject(o->superClassObject, classInfo);
        } else
        {
            fprintf(stderr, "Error: Object not found at reference with class: %s\n", classInfo->getName());
            Platform::exitProgram(-22);
        }
    }
    return o;
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

FieldData* Object::getField(const char* name, const char* descriptor, JavaHeap* heap)
{
    bool found = false;
    for (u2 currentField = 0; currentField < fieldsCount; ++currentField)
    {
        const char* targetName = classInfo->constantPool->getString(fields[currentField].nameIndex);
        const char* targetDescriptor = classInfo->constantPool->getString(fields[currentField].descriptorIndex);

        if (strcmp(name, targetName) == 0 && strcmp(targetDescriptor, descriptor) == 0)
        {
            found = true;
            return &fields[currentField];
        }
    }
    if (!found)
    {
        if (superClassObject != 0)
        {
            return heap->getObject(superClassObject)->getField(name, descriptor, heap);
        }
    }
    fprintf(stderr, "Error: Field not resolved!");
    Platform::exitProgram(-23);
    return 0;
}
