#pragma once

#include "Core.h"
#include "DynamicArray.h"
#include "Data/Class.h"
#include "Data/Variable.h"

#include <vector>

class VM;
class JavaHeap;

enum ReferenceType : char {
    OBJECT, ARRAY, CLASSOBJECT
};

class Object;
class Array;

class Reference {
public:
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
    FieldData* fields{nullptr};
    uint16_t fieldsCount{0};
    ClassInfo* classInfo{nullptr};
    u4 superClassObject{0};
    FieldData* getField(const char* name, const char* descriptor, JavaHeap* heap) const;
};

class ClassObject : public Object
{
public:
    ClassInfo* targetClassInfo{nullptr};
};

class Array : public Reference {
public:
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

    /*
     * Object operations
     */
    uint32_t createArray(ArrayType type, uint64_t size);
    uint32_t createObject(ClassInfo* classInfo, VM* VM);
    uint32_t createClassObject(ClassInfo* classInfo, VM* VM, ClassInfo* targetClassInfo);
    uint32_t createString(const char* utf8String, VM* VM);

    [[nodiscard]] Object* getObject(uint32_t id) const;
    [[nodiscard]] ClassObject* getClassObject(uint32_t id) const;
    [[nodiscard]] Object* getChildObject(uint32_t id, ClassInfo* classInfo);
    [[nodiscard]] Array* getArray(u4 id) const;
    [[nodiscard]] u4 getString(const char* utf8String) const;

    /*
     * Class Operations
     */
    void addClassInfo(ClassInfo* classInfo);
    ClassInfo* getClassByName(const char* className);
};
