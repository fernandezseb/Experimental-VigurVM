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

#include "Platform.h"

#include <Windows.h>

#include "Util.h"

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

void* Platform::allocateMemory(size_t size, size_t baseAddress)
{
	LPVOID lpBaseAddress = (LPVOID)baseAddress;
	return VirtualAlloc(
		lpBaseAddress, 
		size, 
		MEM_COMMIT, 
		PAGE_READWRITE);
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

void Platform::cleanup()
{
}
