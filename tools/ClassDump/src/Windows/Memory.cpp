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
