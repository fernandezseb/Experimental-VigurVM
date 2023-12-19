
#pragma once

#include <cstddef>


class OutputBuffer {
private:
    size_t bufferSize;
    size_t currentSize;
    char* buffer;
public:
    explicit OutputBuffer(size_t bufferSize);
    ~OutputBuffer();
    void print(char* string, size_t length);
    void flush();
};


