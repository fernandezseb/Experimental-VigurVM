#pragma once

#include <string_view>

#include "Core.h"

std::string_view modifiedUtf8ToStandardUtf8(const char *input, const char* memory);

std::u16string_view u8ToU16String(const char* utf8String);

const char* u16StringToU8String(std::u16string_view u16String);