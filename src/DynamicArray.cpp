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

#include "DynamicArray.h"

#include "Platform.h"
#include "Data/Class.h"
#include "VM/Native.h"

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
T DynamicArray<T>::get(size_t index) const
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
template class DynamicArray<NativeMethod>;