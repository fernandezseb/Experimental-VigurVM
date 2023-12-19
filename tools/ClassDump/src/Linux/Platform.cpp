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


struct PlatformFile {
	int fd;
	const char* name;
	uint8_t* fileMemory;
};


void Platform::initialize()
{
	textBuffer = (char*) allocateMemory(getPageSize(), 0);
	outBuffer = new OutputBuffer(getPageSize());
}


void* Platform::allocateMemory(size_t size, size_t baseAddress)
{
	return malloc(size);
}

void Platform::freeMemory(void* allocatedMemory)
{
	free(allocatedMemory);
}

PlatformFile* Platform::getFile(const char* name)
{
	PlatformFile *file = (PlatformFile*) allocateMemory(sizeof(PlatformFile), 0);

	file->fd = open(name, O_RDONLY);
	file->name = name;
	file->fileMemory = NULL;

	return file;
}

char* Platform::getFullPath(PlatformFile* file, char* charOut)
{
	char* absolutePath = realpath(file->name, NULL);	

	strcpy(charOut, absolutePath);

	free(absolutePath);

	return charOut;
}

uint8_t* Platform::readEntireFile(PlatformFile* file, size_t* sizeOut)
{
	struct stat st;
	stat(file->name, &st);
	uint64_t size = st.st_size;
	
	*sizeOut = size;

	uint8_t* fileMemory = (uint8_t*)Platform::allocateMemory(size, 0);
	file->fileMemory = fileMemory;

	uint64_t bytesRead = read(file->fd, fileMemory, size);


	return fileMemory;
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

void Platform::printModifiedUtf8String(char* string)
{
	JString jstring = {0};
	size_t length = strlen(string);
	jstring.chars = (char*) allocateMemory(length, 0); // In the worst case, the string has the same length
	jstring.length = length;
	modifiedUtf8ToStandardUtf8(string, &jstring);
	outBuffer->print(jstring.chars, jstring.length);
	freeMemory(jstring.chars);
}

int Platform::printModifiedUtf8StringFormatted(const char* string, ...)
{
	va_list argsList;
	va_start(argsList, string);
	textBuffer[0] = 0;
	int size = vsnprintf(textBuffer, getPageSize(), string, argsList);
	outBuffer->print(textBuffer, size);
	va_end(argsList);

	return size;
}

void Platform::flush()
{
	outBuffer->flush();
}

void Platform::closeFile(PlatformFile* file)
{
	close(file->fd);
	freeMemory(file);
}

void Platform::exitProgram(uint32_t exitCode)
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
	if (outBuffer != nullptr)
	{
		delete outBuffer;
		outBuffer = nullptr;
	}
}
