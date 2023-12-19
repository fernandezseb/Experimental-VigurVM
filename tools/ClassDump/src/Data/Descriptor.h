#pragma once

#include "Core.h"

struct Descriptor {
	char** args;
	uint16_t argsCount;
	char* returnType;
};