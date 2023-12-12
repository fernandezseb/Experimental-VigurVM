#pragma once


class FieldInfo;

enum VariableType
{
    VariableType_UNDEFINED,
    VariableType_BOOLEAN,
    VariableType_CHAR,
    VariableType_REFERENCE,
    VariableType_BYTE,
    VariableType_INT,
    VariableType_FLOAT,
    VariableType_LONG
};

enum ArrayType : char {
    AT_BOOLEAN,
    AT_BYTE,
    AT_CHAR,
    AT_SHORT,
    AT_INT,
    AT_LONG, // 2 are needed for this
    AT_REFERENCE,
    AT_UNDEFINED,
    AT_END
};

struct Variable
{
    VariableType type;
    u4 data;
};

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
    Variable* Data;
};

// TODO: Add superclass references
class Object : public Reference {
public:
    Object() : Reference(OBJECT) {
    };
    FieldData** fields;
    uint16_t fieldsCount;
};

class Array : public Reference {
public:
    Array() : Reference(ARRAY), arrayType(AT_UNDEFINED), length(0l), data(0) {
    };
    ArrayType arrayType;
    uint64_t length;
    uint32_t* data;
};