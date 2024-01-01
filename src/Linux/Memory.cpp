#include "Memory.h"

Memory::Memory(size_t size, size_t maxSize) 
	: m_size(size), m_maxSize(maxSize)
{
	m_size = maxSize;
	m_memoryPtr = (uint8_t*)Platform::allocateMemory(m_maxSize, 0);
}

Memory::~Memory()
{
	Platform::freeMemory(m_memoryPtr);
}

void* Memory::alloc(size_t size)
{
	if (m_ptr + size > m_size) {
		fprintf(stderr, "\nOut of memory\n");
		printSize();
		Platform::exitProgram(3);
	}

	size_t oldPtr = m_ptr;
	m_ptr += size;
	return m_memoryPtr + oldPtr;
}

void Memory::printSize()
{
	printf("Memory stats:\n");
	printf("  Memory used: %zu bytes\n", m_ptr);
	printf("  Free memory: %zu bytes\n", (m_size-m_ptr));
}
