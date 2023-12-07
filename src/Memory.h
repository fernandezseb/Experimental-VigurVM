#pragma once

#include "Core.h"
#include "Platform.h"

class Memory {
private:
	uint8_t* memoryPtr;
	size_t ptr = 0;
	size_t size;
	size_t maxSize;
	size_t pageSize;
public:
	Memory(size_t size, size_t maxSize);
	~Memory();

	void* alloc(size_t size);
	void printSize();
};