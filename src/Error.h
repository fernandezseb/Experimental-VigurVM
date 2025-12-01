#pragma once

#include "Core.h"

enum ErrorCode : i4
{
    NATIVE_METHOD_NOT_FOUND = -63,
    TYPE_MISMATCH = -64,
    INVALID_MEMORY_SIZE = -65,
    OUT_OF_MEMORY = -66
};