
#pragma once

enum VariableType
{
    VariableType_UNDEFINED,
    VariableType_REFERENCE,
    VariableType_BYTE,
    VariableType_INT,
    VariableType_FLOAT,
    VariableType_LONG
};

struct Variable
{
    VariableType type;
    u4 data;
};
