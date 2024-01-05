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
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "Core.h"
#include "Util.h"

void Platform::initialize()
{
	textBuffer = (char*) allocateMemory(getPageSize(), 0);
}


void* Platform::allocateMemory(size_t size, size_t baseAddress)
{
	return malloc(size);
}

void Platform::freeMemory(void* allocatedMemory)
{
	free(allocatedMemory);
}

void Platform::getLastModifiedString(PlatformFile* file, char* stringOut)
{

	char time[50];
	struct stat attrib;
    stat(file->name, &attrib);
	strftime(time, 20, "%d-%m-%y", localtime(&(attrib.st_ctime)));
	strcpy(stringOut, time);
}

void Platform::print(const char* string, uint64_t length)
{
	fwrite(string, 1 ,length, stdout);
}

void Platform::exitProgram(const int32_t exitCode)
{
	cleanup();
	exit(exitCode);
}

size_t Platform::getPageSize()
{
	return getpagesize();
}

void Platform::cleanup()
{
	if (textBuffer != nullptr) {
		freeMemory(textBuffer);
		textBuffer = nullptr;
	}
}
