/*
 * Copyright (c) 2023-2025 Sebastiaan Fernandez.
 *
 * This file is part of VigurVM.
 *
 * VigurVM is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * VigurVM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with VigurVM.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "DescriptorParser.h"
#include "CoreUtils.h"

char* DescriptorParser::getArgsPart(char* descriptor, uint16_t* length)
{
	char* openBracket = descriptor + 1;
	char* closeBracket = strchr(descriptor, ')');

	*length = castToU2<std::size_t>(closeBracket - openBracket);

	return openBracket;
}

char* DescriptorParser::getReturnPart(char* descriptor, uint16_t* length)
{
	char* closeBracket = strchr(descriptor, ')');
	*length = castToU2<std::size_t>((descriptor + (strlen(descriptor)-1)) - closeBracket);
	return  closeBracket+1;
}

static uint16_t getDescriptorArgsCount(char* descriptorPart, uint16_t length)
{
	size_t i = 0;
	uint16_t size = 0;

	while ((i < length) && (descriptorPart[i] != 0)) {
		char c = descriptorPart[i];
		if (c == 'L') {
			char* semiColonLocation = strchr(&descriptorPart[i], ';');
			size_t classLength = semiColonLocation - &descriptorPart[i] + 1;
			i += classLength;
			size++;
		}
		else if (c == '[') {
			++i;
		}
		else {
			size++;
			i++;
		}
	}

	return size;
}

char** DescriptorParser::getTypes(char* descriptorPart, uint16_t partLength, uint16_t* size, Memory* memory)
{

	uint16_t argsCount = getDescriptorArgsCount(descriptorPart, partLength);
	char** parts = (char**) memory->alloc(sizeof(char*)*argsCount);

	uint16_t i = 0;
	*size = 0;

	int arrayCount = 0;

	while ((i < partLength) && (descriptorPart[i] != 0)) {
		char c = descriptorPart[i];
		if (c == 'L') {
			char* semiColonLocation = strchr(&descriptorPart[i], ';');
			size_t classLength = semiColonLocation - &descriptorPart[i]+1;

			char* classStr = (char*) memory->alloc(classLength+1 + arrayCount);
			memcpy(&classStr[arrayCount], &descriptorPart[i], classLength);
			classStr[arrayCount+classLength] = 0;
			for (int currentArr = 0; currentArr < arrayCount; ++currentArr) {
				classStr[currentArr] = '[';
			}
			parts[*size] = classStr;

			arrayCount = 0;
			i += castToU2<std::size_t>(classLength);
			(*size)++;
		}
		else if (c == '[') {
			++arrayCount;
			++i;
		}
		else {
			char* arg = (char*) memory->alloc(2 + arrayCount);
			arg[arrayCount + 0] = c;
			arg[arrayCount + 1] = 0;
			for (int currentArr = 0; currentArr < arrayCount; ++currentArr) {
				arg[currentArr] = '[';
			}
			parts[*size] = arg;

			arrayCount = 0;
			(*size)++;
			i++;
		}
	}

	return parts;
}

Descriptor DescriptorParser::parseDescriptor(char* descriptor, Memory* memory)
{
	Descriptor desc;
	
	uint16_t sizeArgs;
	uint16_t sizeReturnType;

	uint16_t argsPartLength;
	char* argsPart = getArgsPart(descriptor, &argsPartLength);
	desc.args = getTypes(argsPart, argsPartLength, &sizeArgs, memory);

	uint16_t returnPartLength;
	char* returnPart = getReturnPart(descriptor, &returnPartLength);
	desc.returnType = getTypes(returnPart, returnPartLength, &sizeReturnType, memory)[0];

	desc.argsCount = sizeArgs;

	return desc;
}
