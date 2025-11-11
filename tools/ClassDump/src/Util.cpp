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

#include "Util.h"

void joinStrings(char** strings, size_t arraySize, const char* delim, size_t bufferSize, char* buffer)
{
	if (arraySize == 0) {
		buffer[0] = 0;
		return;
	}

	strcpy(buffer, strings[0]);

	if (arraySize > 1) {
		size_t index = 1;
		while (index < arraySize) {
			strcat(buffer, delim);
			strcat(buffer, strings[index]);
			++index;
		}
	}
}

void modifiedUtf8ToStandardUtf8(char *input, JString *jstring)
{
	int currentOut = 0;
	for (int i = 0; i < strlen(input); ++i)
	{
		if (((uint8_t)input[i]) == 0xED)
		{
			uint32_t codepoint = 0x10000 +
						   ((input[i + 1] & 0x0f) << 16) +
						   ((input[i + 2] & 0x3f) << 10) +
						   ((input[i + 4] & 0x0f) << 6) +
						   (input[i + 5] & 0x3f);
			char fourBytes[5];
			jstring->chars[currentOut++] = (char)(((codepoint >> 18) & 0x07) | 0xF0);
			jstring->chars[currentOut++] = (char)(((codepoint >> 12) & 0x3F) | 0x80);
			jstring->chars[currentOut++] = (char)(((codepoint >> 6) & 0x3F) | 0x80);
			jstring->chars[currentOut++] = (char)(((codepoint >> 0) & 0x3F) | 0x80);
			i += 5;
		} else if ((((uint8_t)input[i]) == 0xC0)&&(((uint8_t)input[i+1]) == 0x80) ) {
			jstring->chars[currentOut++] = 0;
		} else {
			jstring->chars[currentOut++] = input[i];
		}
	}

	jstring->length = currentOut;
}