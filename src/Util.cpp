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

std::string_view modifiedUtf8ToStandardUtf8(const char *input, char* outputMemory)
{
	std::size_t currentOut = 0;
	for (int i = 0; i < strlen(input); ++i)
	{
		if (static_cast<uint8_t>(input[i]) == 0xED)
		{
			const uint32_t codepoint = 0x10000 +
						   ((input[i + 1] & 0x0f) << 16) +
						   ((input[i + 2] & 0x3f) << 10) +
						   ((input[i + 4] & 0x0f) << 6) +
						   (input[i + 5] & 0x3f);
			outputMemory[currentOut++] = static_cast<char>(codepoint >> 18 & 0x07 | 0xF0);
			outputMemory[currentOut++] = static_cast<char>(codepoint >> 12 & 0x3F | 0x80);
			outputMemory[currentOut++] = static_cast<char>(codepoint >> 6 & 0x3F | 0x80);
			outputMemory[currentOut++] = static_cast<char>(codepoint >> 0 & 0x3F | 0x80);
			i += 5;
		} else if (static_cast<uint8_t>(input[i]) == 0xC0 && static_cast<uint8_t>(input[i + 1]) == 0x80 ) {
			outputMemory[currentOut++] = 0;
		} else {
			outputMemory[currentOut++] = input[i];
		}
	}

	return std::string_view{outputMemory, currentOut};
}
