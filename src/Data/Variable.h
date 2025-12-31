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

typedef i4 vint;
typedef i8 vlong;
typedef u4 vreference;
typedef double vdouble;
typedef float vfloat;

union vvalue
{
    vint i;
    vlong j;
    vreference l;
    vdouble d;
    vfloat f;
};



[[nodiscard]] VariableType fromDescriptor(std::string_view descriptor) noexcept;
[[nodiscard]] u1 getCategoryFromVariableType(VariableType type) noexcept;
[[nodiscard]] u1 getDescriptorVarCategory(std::string_view descriptor) noexcept;

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

struct vdata
{
    VariableType type;
    vvalue value;

    explicit constexpr vdata(const VariableType type) noexcept
        : type(type), value(0)
    {
    }

    explicit constexpr vdata(const VariableType type, const vvalue value) noexcept
        : type(type), value(value)
    {
    }

    explicit constexpr vdata(const VariableType type, const vint data) noexcept
        : type(type), value(data)
    {
        checkType(VariableType_INT);
    }

    explicit constexpr vdata(const VariableType type, const vlong data) noexcept
        : type(type)
    {
        checkType(VariableType_LONG);
        value.j = data;
    }

    explicit constexpr vdata(const VariableType type, const vreference data) noexcept
        : type(type)
    {
        checkType(VariableType_REFERENCE);
        value.l = data;
    }

    explicit constexpr vdata(const VariableType type, const vfloat data) noexcept
        : type(type)
    {
        checkType(VariableType_FLOAT);
        value.f = data;
    }

    explicit constexpr vdata(const VariableType type, const vdouble data) noexcept
        : type(type)
    {
        checkType(VariableType_DOUBLE);
        value.f = data;
    }

    [[nodiscard]] constexpr u1 getCategory() const noexcept
    {
        if (type == VariableType_DOUBLE || type == VariableType_LONG)
        {
            return 2;
        }

        return 1;
    }

    [[nodiscard]] vreference getReference() const noexcept
    {
        checkType(VariableType_REFERENCE);
        return value.l;
    }

    [[nodiscard]] vint getInt() const noexcept
    {
        checkType(VariableType_INT);
        return value.i;
    }

    [[nodiscard]] vlong getLong() const noexcept
    {
        checkType(VariableType_LONG);
        return value.j;
    }

    [[nodiscard]] vdouble getDouble() const noexcept
    {
        checkType(VariableType_DOUBLE);
        return value.d;
    }

    [[nodiscard]] vfloat getFloat() const noexcept
    {
        checkType(VariableType_FLOAT);
        return value.f;
    }

    void checkType(VariableType type) const;
};
