#include "JavaHeap.h"

#include "Util.h"
#include "VM.h"

JavaHeap::JavaHeap()
{
    // We don't want the heap to return reference 0,
    // because this would be considered a null pointer
    objects.push_back(0);
}

uint32_t JavaHeap::createArray(ArrayType type, uint64_t size)
{
    Array* array = static_cast<Array*>(Platform::allocateMemory(sizeof(Array), 0));
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
    return castToU4<std::size_t>(objects.size()-1);
}

uint32_t JavaHeap::createObject(ClassInfo* classInfo, VM* VM)
{
    // TODO: Fix undefined behavior with uninitialized memory in object
    // We are NOT calling the constructor of the object here
    Object* object = (Object*) Platform::allocateMemory(sizeof(Object), 0);
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
    object->fields = 0;
    if (fieldsCount > 0)
    {
        object->fields = (FieldData*) Platform::allocateMemory(sizeof(FieldData) * fieldsCount, 0);
    }
    object->fieldsCount = fieldsCount;
    object->superClassObject = 0;

    fieldsCount = 0;
    for (u2 currentField = 0; currentField < classInfo->fieldsCount; ++currentField)
    {
        FieldInfo* fieldInfo = classInfo->fields[currentField];
        if (!fieldInfo->isStatic())
        {
            FieldData data = {};
            data.descriptorIndex = fieldInfo->descriptorIndex;
            data.nameIndex = fieldInfo->nameIndex;
            const char* descriptorText = classInfo->constantPool->getString(fieldInfo->descriptorIndex);
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
        const char* superClassName = classInfo->constantPool->getString(classInfo->constantPool->getClassInfo(classInfo->superClass)->nameIndex);
        if (strcmp(superClassName, "java/lang/Object") != 0)
        {
            ClassInfo* superClass = getClassByName(superClassName);
            u4 superClassObject = createObject(superClass, VM);
            object->superClassObject = superClassObject;
        }
    }

    objects.push_back(object);
    return castToU4<std::size_t>(objects.size()-1);
}

// TODO: De-duplicate code from createObject
uint32_t JavaHeap::createClassObject(ClassInfo* classInfo, VM* VM, ClassInfo* targetClassInfo)
{
    // TODO: Fix undefined behavior with uninitialized memory in object
    // We are NOT calling the constructor of the object here
    ClassObject* object = (ClassObject*) Platform::allocateMemory(sizeof(ClassObject), 0);
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
    object->type = CLASSOBJECT;
    object->fields = 0;
    if (fieldsCount > 0)
    {
        object->fields = (FieldData*) Platform::allocateMemory(sizeof(FieldData) * fieldsCount, 0);
    }
    object->fieldsCount = fieldsCount;
    object->superClassObject = 0;
    object->targetClassInfo = targetClassInfo;

    fieldsCount = 0;
    for (u2 currentField = 0; currentField < classInfo->fieldsCount; ++currentField)
    {
        FieldInfo* fieldInfo = classInfo->fields[currentField];
        if (!fieldInfo->isStatic())
        {
            FieldData data = {};
            data.descriptorIndex = fieldInfo->descriptorIndex;
            data.nameIndex = fieldInfo->nameIndex;
            const char* descriptorText = classInfo->constantPool->getString(fieldInfo->descriptorIndex);
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
        const char* superClassName = classInfo->constantPool->getString(classInfo->constantPool->getClassInfo(classInfo->superClass)->nameIndex);
        if (strcmp(superClassName, "java/lang/Object") != 0)
        {
            ClassInfo* superClass = getClassByName(superClassName);
            u4 superClassObject = createObject(superClass, VM);
            object->superClassObject = superClassObject;
        }
    }

    objects.push_back(object);
    return static_cast<u4>(objects.size()-1);
}


uint32_t JavaHeap::createString(const char* utf8String, VM* VM) {
    const u4 existingString = getString(utf8String);
    if (existingString != 0) {
        return existingString;
    }

    const u4 strObjectId = createObject(getClassByName("java/lang/String"), VM);
    const Object* strObject = getObject(strObjectId);

    const u4 arrId = createArray(AT_CHAR, strlen(utf8String));
    const Array* charArray = getArray(arrId);
    memcpy((char*)charArray->data, utf8String, strlen(utf8String));

    const Variable var{VariableType_REFERENCE, arrId};
    strObject->fields[0].data[0] = var;

    return strObjectId;
}

Object* JavaHeap::getObject(const uint32_t id) const
{
    if (id == 0)
    {
        // Nullpointer
        fprintf(stderr, "Error: Null pointer exception!\n");
        Platform::exitProgram(1);
    }
    Reference* ref = objects[id];
    if (ref->type == OBJECT)
    {
        return static_cast<Object*>(objects[id]);
    } else
    {
        fprintf(stderr, "Error: Array instead of Object\n");
        Platform::exitProgram(22);
    }
    return nullptr;
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
            Platform::exitProgram(22);
        }
    }
    return o;
}

Array* JavaHeap::getArray(const uint32_t id) const
{
    if (id == 0)
    {
        // Nullpointer
        fprintf(stderr, "Error: Null pointer exception!");
        Platform::exitProgram(1);
    }
    Reference* ref = objects[id];
    if (ref->type == ARRAY)
    {
        return static_cast<Array*>(objects[id]);
    } else
    {
        fprintf(stderr, "Error: Array instead of Object");
        Platform::exitProgram(22);
    }
    return nullptr;
}

u4 JavaHeap::getString(const char* utf8String) const
{
    for (uint32_t currentObj = 1; currentObj < objects.size(); ++currentObj)
    {
        const Reference* ref = objects[currentObj];
        if (ref->type == ReferenceType::OBJECT) {
            const Object* obj = static_cast<const Object*>(ref);
            if (strcmp(obj->classInfo->getName(), "java/lang/String") == 0) {
                Variable charArrRef =  obj->fields[0].data[0];
                Array* arr = getArray(charArrRef.data);
                if (strncmp((const char*)arr->data, utf8String,
                    (arr->length > strlen(utf8String) ? arr->length : strlen(utf8String))) == 0) {
                    return currentObj;
                }
            }
        }
    }
    return 0;
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
    return nullptr;
}

FieldData* Object::getField(const char* name, const char* descriptor, JavaHeap* heap) const
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
    if (!found && superClassObject != 0)
    {
        return heap->getObject(superClassObject)->getField(name, descriptor, heap);
    }
    fprintf(stderr, "Error: Field not resolved!");
    Platform::exitProgram(23);
    return nullptr;
}
