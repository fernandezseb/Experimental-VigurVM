#pragma once

#include "Core.h"

template <class T>
class DynamicArray
{
private:
    size_t maxSize;
    size_t size;
    T* array;
    void expand();
public:
    explicit DynamicArray(size_t size);
    T get(size_t index) const;
    int set(size_t index, T element);
    void add(T element);
    [[nodiscard]] size_t getSize() const;
    ~DynamicArray();
};

