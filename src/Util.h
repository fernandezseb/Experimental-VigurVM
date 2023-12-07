#pragma once

#include "Core.h"

void joinStrings(char** strings, size_t arraySize, const char* delim, size_t bufferSize, char* buffer);
void modifiedUtf8ToStandardUtf8(char *input, JString *jstring);