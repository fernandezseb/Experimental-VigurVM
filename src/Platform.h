#pragma once

#include <cinttypes>
#include <cstddef>
#include <cstdarg>

#include "OutputBuffer.h"

struct PlatformFile;

class Platform {
private:
	inline static char* textBuffer = nullptr;
public:
	static void print(const char* string, uint64_t length);
	static void initialize();
	static void* allocateMemory(size_t size, size_t baseAddress);
	static void freeMemory(void* allocatedMemory);
	static PlatformFile* getFile(const char* name);
	static char* getFullPath(PlatformFile* file, char* charOut);
	static uint8_t* readEntireFile(PlatformFile* file, size_t* sizeOut);
	static void getLastModifiedString(PlatformFile* file, char* stringOut);
	static void closeFile(PlatformFile* file);
	static void exitProgram(uint32_t exitCode);
	static size_t getPageSize();
	static void cleanup();
	friend class OutputBuffer;
};