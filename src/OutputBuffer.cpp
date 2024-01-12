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

#include "OutputBuffer.h"

#include <Platform.h>

#include <cstring>

OutputBuffer::OutputBuffer(size_t bufferSize)
    : bufferSize(bufferSize), currentSize(0)
{
    buffer = (char*) Platform::allocateMemory(bufferSize, 0);
}

OutputBuffer::~OutputBuffer()
{
    Platform::freeMemory(buffer);
}

void OutputBuffer::print(char* string, size_t length)
{
    if (currentSize + length > bufferSize)
    {
        flush();
    }

    memcpy(&buffer[currentSize], string, length);
    currentSize += length;
}

void OutputBuffer::flush()
{
    Platform::print(buffer, currentSize);
    currentSize = 0;
    memset(buffer, 0, bufferSize);
}
