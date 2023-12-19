#include "../Memory.h"
#include "windows.h"

static void* reserveMemory(size_t size, size_t baseAddress) {
	LPVOID lpBaseAddress = (LPVOID)baseAddress;
	return VirtualAlloc(
		lpBaseAddress,
		size,
		MEM_RESERVE,
		PAGE_READWRITE);
}

Memory::Memory(size_t size, size_t maxSize) 
	: size(size), maxSize(maxSize)
{
	pageSize = Platform::getPageSize();

	// We want to at least allocate one page if possible
	if (this->size < pageSize) {
		this->size = pageSize;

		// Make sure that we don't allocate more than the max allowed
		if (this->size > maxSize) {
			this->size = maxSize;
		}
	}

	uint8_t* reservedMemory = (uint8_t*)reserveMemory(maxSize, 0);
	memoryPtr = (uint8_t*)Platform::allocateMemory(this->size, (size_t)reservedMemory);
}

Memory::~Memory()
{
	Platform::freeMemory(memoryPtr);
}

void* Memory::alloc(size_t size)
{

	if (ptr + size > this->size) {
		size_t toAlloc = pageSize;
		if (this->size + pageSize > maxSize) {
			toAlloc = maxSize - this->size;
		}

		Platform::allocateMemory(toAlloc, ((size_t)memoryPtr)+this->size);
		this->size += toAlloc;
	}

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
	jprintf("  Memory commited: %zu bytes\n", size);
	jprintf("  Free memory: %zu bytes\n", (size-ptr));
	jprintf("  Max memory (reserved): %zu bytes\n", maxSize);
	jflush();
}
