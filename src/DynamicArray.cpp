#include "DynamicArray.h"

#include "Platform.h"
#include "Data/Class.h"

template <class T>
void DynamicArray<T>::expand()
{
    T* array2 = (T*) Platform::allocateMemory(sizeof(T) * maxSize * 2, 0);
    memcpy(array2, array, maxSize);
    Platform::freeMemory(array);
    array = array2;
    maxSize *= 2;
}

template <class T>
DynamicArray<T>::DynamicArray(size_t size) :
maxSize(size), size(0)
{
    array = (T*) Platform::allocateMemory(sizeof(T) * size, 0);
}

template <class T>
T DynamicArray<T>::get(size_t index)
{
    return array[index];
}

template <class T>
int DynamicArray<T>::set(size_t index, T element)
{
    if (index >= size)
    {
        return -1;
    }
    array[index] = element;
    return 0;
}

template <class T>
void DynamicArray<T>::add(T element)
{
    if (size >= maxSize)
    {
        expand();
    }
    array[size++] = element;
}

template <class T>
size_t DynamicArray<T>::getSize() const
{
    return size;
}

template <class T>
DynamicArray<T>::~DynamicArray()
{
    Platform::freeMemory(array);
}

template class DynamicArray<uint32_t>;
template class DynamicArray<ClassInfo*>;