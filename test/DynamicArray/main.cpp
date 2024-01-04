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

int main(int argc, char* argv[])
{
    DynamicArray<uint32_t> intArray(5);
    intArray.add(2);
    intArray.add(3);
    intArray.add(2);
    intArray.add(4);
    intArray.add(2);
    intArray.add(3);
    if (intArray.getSize() != 6)
    {
        return -1;
    }

    if (intArray.get(0) != 2)
    {
        return -2;
    }

    if (intArray.get(1) != 3)
    {
        return -3;
    }
    if (intArray.get(5) != 3)
    {
        return -4;
    }
    if (intArray.set(6, 10) >= 0)
    {
        return -4;
    }
    return 0;
}
