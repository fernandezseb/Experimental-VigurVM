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

#include "ByteArray.h"

ByteArray::ByteArray(uint8_t* bytes, uint64_t size) :
	bytes(bytes), size(size)
{
}

uint8_t ByteArray::readUnsignedByte()
{
	uint8_t buffer = 0;
	buffer = bytes[bytePtr++];
	return buffer;
}

uint16_t ByteArray::readUnsignedShort()
{
	uint8_t buffer[2] = { bytes[bytePtr++], bytes[bytePtr++] };
	uint16_t value = (uint16_t)buffer[1] | (uint16_t)buffer[0] << 8;
	return value;
}

uint32_t ByteArray::readUnsignedInt()
{
	uint8_t buffer[4] = { bytes[bytePtr++], bytes[bytePtr++] , bytes[bytePtr++] , bytes[bytePtr++] };
	uint32_t value = (uint32_t)buffer[3] | (uint32_t)buffer[2] << 8 | (uint32_t)buffer[1] << 16 | (uint32_t)buffer[0] << 24;
	return (uint32_t)value;
}

int32_t ByteArray::readSignedInt()
{
	uint8_t buffer[4] = { bytes[bytePtr++], bytes[bytePtr++] , bytes[bytePtr++] , bytes[bytePtr++] };
	int32_t value = (int32_t)buffer[3] | (int32_t)buffer[2] << 8 | (int32_t)buffer[1] << 16 | (int32_t)buffer[0] << 24;
	return (int32_t)value;
}

void ByteArray::readBytes(uint8_t* target, uint64_t size)
{
	memcpy(target, &bytes[bytePtr], sizeof(uint8_t) * size);
	bytePtr += size;
}

void ByteArray::setPtr(uint64_t ptr)
{
	this->bytePtr = ptr;
}

uint64_t ByteArray::getPtr()
{
	return bytePtr - 1;
}

bool ByteArray::atEnd()
{
	return (bytePtr == size);
}

uint64_t ByteArray::getSize()
{
	return size;
}
