#pragma once

#define __STDC_FORMAT_MACROS

#ifdef _CPPRTTI
#error "RTTI enabled but not required for this project"
#endif

#include <cstdio>
#include <cstring>
#include <cinttypes>
#include <cstddef>

#define CCALL extern "C"

// Type strings
#define T_VOID    "void"
#define T_BYTE    "byte"
#define T_CHAR    "char"
#define T_DOUBLE  "double"
#define T_FLOAT   "float"
#define T_INT     "int"
#define T_LONG    "long"
#define T_SHORT   "short"
#define T_BOOLEAN "boolean"
#define T_DOUBLE  "double"

// Fundamental JVM sizes
typedef uint8_t  u1;
typedef uint16_t u2;
typedef uint32_t u4;
typedef uint64_t u8;

typedef int8_t  i1;
typedef int16_t i2;
typedef int32_t i4;
typedef int64_t i8;

// Memory sizes
#define KIB(x)   ((size_t) (x) << 10)
#define MIB(x)   ((size_t) (x) << 20)

/*
 * A string containing real UTF-8 characters.
 * Not like the ones in class files
 */
struct JString
{
    uint64_t length;
    char* chars;
};