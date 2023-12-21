#pragma once

#include "Core.h"
#include "DynamicArray.h"
#include "Data/Class.h"
#include "Data/Variable.h"

#include <vector>

class VM;
class JavaHeap;

enum ReferenceType : char {
    OBJECT, ARRAY
};

class Object;
class Array;

class Reference {
public:
    explicit Reference(ReferenceType type) { this->type = type; };
    ReferenceType type;

    Array* getArray() {
        if (type == ARRAY) {
            return (Array*)this;
        }
        else {
            fprintf(stderr, "Error: Reference is not of type 'Array'");
            Platform::exitProgram(-23);
        }
        return nullptr;
    };

    Object* getObject() {
        if (type == OBJECT) {
            return (Object*)this;
        }
        else {
            fprintf(stderr, "Error: Reference is not of type 'Object'");
            Platform::exitProgram(-23);
        }
        return nullptr;
    };
};

class FieldData {
public:
    uint16_t nameIndex;
    uint16_t descriptorIndex;
    Variable* data;
    u1 dataSize;
};

class Object : public Reference {
public:
    Object() : Reference(OBJECT) {
    };
    FieldData* fields;
    uint16_t fieldsCount;
    ClassInfo* classInfo;
    u4 superClassObject;
    FieldData* getField(const char* name, const char* descriptor, JavaHeap* heap) const;
};

class Array : public Reference {
public:
    Array() : Reference(ARRAY), arrayType(AT_UNDEFINED), length(0l), data(nullptr) {
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
    uint32_t createObject(ClassInfo* classInfo, VM* VM);
    [[nodiscard]] Object* getObject(uint32_t id) const;
    Object* getChildObject(uint32_t id, ClassInfo* classInfo);
    [[nodiscard]] Array* getArray(u4 id) const;
    void addClassInfo(ClassInfo* classInfo);
    ClassInfo* getClassByName(const char* className);
};