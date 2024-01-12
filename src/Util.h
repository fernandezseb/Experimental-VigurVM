#pragma once

#include <cstdlib>
#include <string_view>

#include "Core.h"

std::string_view modifiedUtf8ToStandardUtf8(const char *input, const char* memory);

template <typename T>
[[nodiscard]] static constexpr u1 castToU1(const T value)
{
    if (value > UINT8_MAX)
    {
        fprintf(stderr, "Error: Truncating value when casting to u1\n");
        exit(45);
    }

    return static_cast<u1>(value);
}

template <typename T>
[[nodiscard]] static constexpr u2 castToU2(const T value)
{
    if (value > UINT16_MAX)
    {
        fprintf(stderr, "Error: Truncating value when casting to u2\n");
        exit(46);
    }

    return static_cast<u2>(value);
}

template <typename T>
[[nodiscard]] static constexpr u4 castToU4(const T value)
{
    if (value > UINT16_MAX)
    {
        fprintf(stderr, "Error: Truncating value when casting to u4\n");
        exit(47);
    }

    return static_cast<u4>(value);
}