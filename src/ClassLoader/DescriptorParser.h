#pragma once

#include "Core.h"
#include "Memory.h"
#include "Data/Descriptor.h"


class DescriptorParser {
public:
	static char* getArgsPart(char* descriptor, uint16_t* length);
	static char* getReturnPart(char* descriptor, uint16_t* length);
	static char** getTypes(char* descriptorPart, uint16_t partLength, uint16_t* size, Memory* memory);
	static Descriptor parseDescriptor(char* descriptor, Memory* memory);
};