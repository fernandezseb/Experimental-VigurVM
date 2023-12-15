#pragma once

#include "Core.h"
#include "DynamicArray.h"
#include "Data/Class.h"
#include "Data/Variable.h"

#include <vector>

enum ReferenceType : char {
    OBJECT, ARRAY
};

class Object;
class Array;

class Reference {
public:
    Reference(ReferenceType type) { this->type = type; };
    ReferenceType type;

    Array* getArray() {
        if (type == ARRAY) {
            return (Array*)this;
        }
        else {
            fprintf(stderr, "Error: Reference is not of type 'Array'");
        }
    };

    Object* getObject() {
        if (type == OBJECT) {
            return (Object*)this;
        }
        else {
            fprintf(stderr, "Error: Reference is not of type 'Object'");
        }
    };
};

class FieldData {
public:
    uint16_t nameIndex;
    uint16_t descriptorIndex;
    Variable data; // TODO: Implement this for longs
};

// TODO: Add superclass references
class Object : public Reference {
public:
    Object() : Reference(OBJECT) {
    };
    FieldData* fields;
    uint16_t fieldsCount;
    ClassInfo* classInfo;
    FieldData* getField(const char* name, const char* descriptor)
    {
        for (u2 currentField = 0; currentField < fieldsCount; ++currentField)
        {
            const char* targetName = classInfo->constantPool->getString(fields[currentField].nameIndex);
            const char* targetDescriptor = classInfo->constantPool->getString(fields[currentField].descriptorIndex);

            if (strcmp(name, targetName) == 0 && strcmp(targetDescriptor, descriptor) == 0)
            {
                return &fields[currentField];
            }
        }
        return 0;
    }
};

class Array : public Reference {
public:
    Array() : Reference(ARRAY), arrayType(AT_UNDEFINED), length(0l), data(0) {
    };
    ArrayType arrayType;
    uint64_t length;
    u1* data;
};

class MethodArea {
public:
    MethodArea() : classes(1000) {}
    // Loaded classes to use
    DynamicArray<ClassInfo*> classes;
};

class JavaHeap {
private:
    // List of objects
    std::vector<Reference*> objects;
    MethodArea methodArea;
public:
    JavaHeap();

    uint32_t createArray(ArrayType type, uint64_t size);
    uint32_t createObject(ClassInfo* class_info);
    Object* getObject(uint32_t id);
    Array* getArray(u4 id);
    void addClassInfo(ClassInfo* classInfo);
    ClassInfo* getClassByName(const char* className);
};