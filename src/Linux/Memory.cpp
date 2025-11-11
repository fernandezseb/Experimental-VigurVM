/*
 * Copyright (c) 2023-2025 Sebastiaan Fernandez.
 *
 * This file is part of VigurVM.
 *
 * VigurVM is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * VigurVM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with VigurVM.
 * If not, see <https://www.gnu.org/licenses/>.
 */

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
