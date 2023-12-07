#include "OutputBuffer.h"

#include <cstring>
#include <Platform.h>

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
