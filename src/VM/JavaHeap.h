#pragma once

#include <span>
#include <string_view>

#include "CoreUtils.h"
#include "StringUtils.h"
#include "Core.h"
#include "DynamicArray.h"
#include "Data/Class.h"
#include "Data/VData.h"

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

struct FieldData {
    u1 dataSize;
    uint16_t nameIndex;
    uint16_t descriptorIndex;
    VariableType type{VariableType_UNDEFINED};
    vvalue value;
};

class Object : public Reference {
public:
    std::span<FieldData> fields;
    uint16_t fieldsCount{0};
    // In ClassObjects, this is the classInfo of java/lang/Class
    ClassInfo* classInfo{nullptr};
    u4 superClassObject{0};
    FieldData* getField(const char* name, const char* descriptor) const;
    [[nodiscard]] const Object* getObject(u4 fieldIndex) const;
    [[nodiscard]] const vlong getLong(u4 fieldIndex) const;
};

class ClassObject : public Object
{
public:
    std::string_view name;
    // The ClassInfo of the class to which this "Class" object refers
    ClassInfo* classClassInfo{nullptr};
};

class Array : public Reference {
public:
    ArrayType arrayType;
    uint64_t length;
    u1* data;
    std::string_view descriptor;
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
    ClassInfo* classClassInfo{nullptr};
public:
    JavaHeap();

    void setClassInfo(ClassInfo* classInfo)
    {
        classClassInfo = classInfo;
    }

    /*
     * Object operations
     */
    vreference createArray(ArrayType type, uint64_t size, std::string_view descriptor);
    vreference createObject(ClassInfo* classInfo);
    vreference createClassObject(ClassInfo* classInfo, std::string_view name);
    vreference createString(const char* utf8String);

    [[nodiscard]] const Object* getObject(vreference id) const;
    [[nodiscard]] Reference* getReference(vreference id) const;
    [[nodiscard]] ClassObject* getClassObject(vreference id) const;
    [[nodiscard]] const Object* getChildObject(vreference id, ClassInfo* classInfo);
    [[nodiscard]] const Array* getArray(vreference id) const;
    [[nodiscard]] vreference getString(const char* utf8String) const;
    [[nodiscard]] std::u16string_view getStringContent(const Object* stringObject) const;
    [[nodiscard]] std::u16string_view getStringContent(vreference id) const;
    [[nodiscard]] vreference getClassObjectByName(std::string_view name) const;

    /*
     * Class Operations
     */
    void addClassInfo(ClassInfo* classInfo);
    [[nodiscard]] ClassInfo* getClassByName(std::string_view className) const;
    void printStringPool();
};
