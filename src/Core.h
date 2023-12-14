#pragma once

#define __STDC_FORMAT_MACROS

#ifdef _CPPRTTI
#error "RTTI enabled but not required for this project"
#endif

#include <cstdio>
#include <cstring>
#include <cinttypes>
#include <cstddef>

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
#define u1 uint8_t
#define u2 uint16_t
#define u4 uint32_t

#define i1 int8_t
#define i2 int16_t
#define i4 int32_t

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