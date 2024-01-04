/*
 * Copyright (c) 2023-2024 Sebastiaan Fernandez.
 *
 * This file is part of VigurVM.
 *
 * VigurVM is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * VigurVM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with Foobar.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "Util.h"

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
