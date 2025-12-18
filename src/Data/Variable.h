#pragma once

#include "Core.h"

#include <string_view>


class FieldInfo;

enum VariableType
{
    VariableType_UNDEFINED,
    // VariableType_BOOLEAN,
    // VariableType_CHAR,
    VariableType_REFERENCE,
    // VariableType_BYTE,
    VariableType_INT,
    VariableType_FLOAT,
    VariableType_LONG,
    VariableType_DOUBLE
};

VariableType fromDescriptor(std::string_view descriptor);

u1 getCategoryFromVariableType(const VariableType type);

enum ArrayType : char {
    AT_BOOLEAN = 4,
    AT_BYTE = 8,
    AT_CHAR = 5,
    AT_SHORT = 9,
    AT_INT = 10,
    AT_LONG = 11, // 2 are needed for this
    AT_DOUBLE = 7,
    AT_FLOAT = 6,
    AT_REFERENCE = 1,
    AT_UNDEFINED = -1,
    AT_END
};

struct Variable
{
    VariableType type;
    u4 data;

    explicit constexpr Variable(const VariableType type) noexcept
        : type(type), data(0)
    {
    }

    explicit constexpr Variable(const VariableType type, const u4 data) noexcept
        : type(type), data(data)
    {
    }

    [[nodiscard]] constexpr u1 getCategory() const noexcept
    {
        if (type == VariableType_DOUBLE || type == VariableType_LONG)
        {
            return 2;
        }

        return 1;
    }
};
