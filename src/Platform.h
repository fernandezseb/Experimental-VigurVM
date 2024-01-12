#pragma once

#include <cinttypes>
#include <cstddef>

class Platform {
public:
	static void print(const char* string, uint64_t length);
	static void initialize();
	static void* allocateMemory(size_t size, size_t baseAddress);
	static void freeMemory(void* allocatedMemory);
	static void exitProgram(int32_t exitCode);
	static std::size_t getPageSize();
	static void cleanup();
private:
	inline static char* textBuffer = nullptr;
};