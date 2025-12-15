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

#include "Platform.h"

#include <Windows.h>

#include "CoreUtils.h"
#include "Error.h"

void Platform::initialize()
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	if (hStdout == INVALID_HANDLE_VALUE)
	{
		AllocConsole();
		AttachConsole(ATTACH_PARENT_PROCESS);

		hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

		if (hStdout == INVALID_HANDLE_VALUE)
		{
			exitProgram(6);
		}
	}

	SetConsoleCP(65001);
}

void Platform::print(const char* string, uint64_t length)
{
	WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), string, (DWORD)length, NULL, NULL);
	printf(""); // TODO: Rework printing code
}

void* Platform::allocateMemory(const size_t size, const size_t baseAddress)
{
	if (size <= 0)
	{
		printf("ERROR allocating memory of invalid size! (%llu)\n", size);
		Platform::exitProgram(ErrorCode::INVALID_MEMORY_SIZE);
	}
	const LPVOID lpBaseAddress = (LPVOID)baseAddress;

	void* addr = VirtualAlloc(
		lpBaseAddress, 
		size, 
		MEM_COMMIT, 
		PAGE_READWRITE);
	if (addr == nullptr)
	{
		printf("ERROR allocating memory! \n");
		Platform::exitProgram(ErrorCode::OUT_OF_MEMORY);
	}
	return addr;
}

void Platform::freeMemory(void* allocatedMemory)
{
	VirtualFree(
		allocatedMemory,
		0,
		MEM_RELEASE);
}

void Platform::exitProgram(const int32_t exitCode)
{
	cleanup();
	ExitProcess(exitCode);
}

size_t Platform::getPageSize()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	return si.dwPageSize;
}

void Platform::WinAPISetErrorMode(uint32_t errorMode)
{
	SetErrorMode(errorMode);
}


void Platform::cleanup()
{
}
