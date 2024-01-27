#pragma once

#include <span>
#include <string_view>

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

    [[nodiscard]] const Array* getArray() const {
        if (type == ARRAY) {
            return reinterpret_cast<const Array*>(this);
        }
        fprintf(stderr, "Error: Reference is not of type 'Array'");
        Platform::exitProgram(23);
        return nullptr;
    };

    [[nodiscard]] const Object* getObject() const {
        if (type == OBJECT || type == CLASSOBJECT) {
            return reinterpret_cast<const Object*>(this);
        }
        fprintf(stderr, "Error: Reference is not of type 'Object'");
        Platform::exitProgram(23);
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
    std::span<FieldData> fields;
    uint16_t fieldsCount{0};
    ClassInfo* classInfo{nullptr};
    u4 superClassObject{0};
    FieldData* getField(const char* name, const char* descriptor, JavaHeap* heap) const;
};

class ClassObject : public Object
{
public:
    std::string_view name;
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
    u4 createArray(ArrayType type, uint64_t size);
    u4 createObject(ClassInfo* classInfo, VM* VM);
    u4 createClassObject(ClassInfo* classInfo, VM* VM, std::string_view name);
    u4 createString(const char* utf8String, VM* VM);

    [[nodiscard]] const Object* getObject(uint32_t id) const;
    [[nodiscard]] Reference* getReference(u4 id) const;
    [[nodiscard]] ClassObject* getClassObject(uint32_t id) const;
    [[nodiscard]] const Object* getChildObject(uint32_t id, ClassInfo* classInfo);
    [[nodiscard]] const Array* getArray(u4 id) const;
    [[nodiscard]] u4 getString(const char* utf8String) const;
    [[nodiscard]] u4 getClassObjectByName(std::string_view name) const;

    /*
     * Class Operations
     */
    void addClassInfo(ClassInfo* classInfo);
    [[nodiscard]] ClassInfo* getClassByName(const char* className) const;
};
