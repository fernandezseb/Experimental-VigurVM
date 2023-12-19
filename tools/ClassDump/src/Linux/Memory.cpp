#include "Memory.h"

Memory::Memory(size_t size, size_t maxSize) 
	: size(size), maxSize(maxSize)
{
	this->size = maxSize;
	memoryPtr = (uint8_t*)Platform::allocateMemory(this->maxSize, 0);
}

Memory::~Memory()
{
	Platform::freeMemory(memoryPtr);
}

void* Memory::alloc(size_t size)
{
	if (ptr + size > this->size) {
		fprintf(stderr, "\nOut of memory\n");
		printSize();
		Platform::exitProgram(3);
	}

	size_t oldPtr = ptr;
	ptr += size;
	return memoryPtr + oldPtr;
}

void Memory::printSize()
{
	jprintf("Memory stats:\n");
	jprintf("  Memory used: %zu bytes\n", ptr);
	jprintf("  Free memory: %zu bytes\n", (size-ptr));
}
