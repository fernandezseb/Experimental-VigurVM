#include "Memory.h"
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
	: m_size(size), m_maxSize(maxSize)
{
	m_pageSize = Platform::getPageSize();

	// We want to at least allocate one page if possible
	if (m_size < m_pageSize) {
		m_size = m_pageSize;

		// Make sure that we don't allocate more than the max allowed
		if (m_size > maxSize) {
			m_size = maxSize;
		}
	}

	uint8_t* reservedMemory = (uint8_t*)reserveMemory(maxSize, 0);
	m_memoryPtr = (uint8_t*)Platform::allocateMemory(m_size, (size_t)reservedMemory);
}

Memory::~Memory()
{
	Platform::freeMemory(m_memoryPtr);
}

void* Memory::alloc(size_t size)
{

	if (m_ptr + size > m_size) {
		size_t toAlloc = m_pageSize;
		if (m_size + m_pageSize > m_maxSize) {
			toAlloc = m_maxSize - m_size;
		}

		Platform::allocateMemory(toAlloc, ((size_t)m_memoryPtr)+m_size);
		m_size += toAlloc;
	}

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
	printf("  Memory commited: %zu bytes\n", m_size);
	printf("  Free memory: %zu bytes\n", (m_size-m_ptr));
	printf("  Max memory (reserved): %zu bytes\n", m_maxSize);
}
