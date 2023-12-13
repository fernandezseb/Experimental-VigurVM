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