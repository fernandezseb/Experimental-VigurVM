#pragma once

#include <string_view>

#include "Core.h"

struct J16String
{
    u2* chars;
    u8 length;
};

std::string_view modifiedUtf8ToStandardUtf8(const char *input, const char* memory);

J16String utf8ToJ16String(const char* utf8String);

const char* J16StringToUtf8String(J16String j16String);