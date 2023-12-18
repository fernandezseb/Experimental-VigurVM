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
    VariableType_LONG,
    VariableType_DOUBLE
};

enum ArrayType : char {
    AT_BOOLEAN = 4,
    AT_BYTE = 8,
    AT_CHAR = 5,
    AT_SHORT = 9,
    AT_INT = 10,
    AT_LONG = 11, // 2 are needed for this
    AT_DOUBLE = 7,
    AT_FLOAT = 6,
    AT_REFERENCE,
    AT_UNDEFINED,
    AT_END
};

struct Variable
{
    VariableType type;
    u4 data;
};