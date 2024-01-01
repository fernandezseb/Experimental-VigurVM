#pragma once

#include "Core.h"
#include "Platform.h"

class Memory {
public:
	Memory(size_t size, size_t maxSize);
	~Memory();

	void* alloc(size_t size);
	void printSize();
private:
	uint8_t* m_memoryPtr;
	size_t m_ptr = 0;
	size_t m_size;
	size_t m_maxSize;
	size_t m_pageSize;
};