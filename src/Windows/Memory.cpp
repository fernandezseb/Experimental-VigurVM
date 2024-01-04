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
