#include "ByteArray.h"

ByteArray::ByteArray(u1* bytes, const std::size_t size) :
	size(size), bytes(bytes)
{
}

u1 ByteArray::readUnsignedByte()
{
	const u1 buffer = bytes[bytePtr++];
	return buffer;
}

u2 ByteArray::readUnsignedShort()
{
	const u1 buffer[2] = { bytes[bytePtr++], bytes[bytePtr++] };
	const uint16_t value = static_cast<uint16_t>(buffer[1]) | static_cast<uint16_t>(buffer[0]) << 8;
	return value;
}

uint32_t ByteArray::readUnsignedInt()
{
	const uint8_t buffer[4] = {bytes[bytePtr++], bytes[bytePtr++], bytes[bytePtr++], bytes[bytePtr++]};
	const uint32_t value = static_cast<uint32_t>(buffer[3])
	                       | static_cast<uint32_t>(buffer[2]) << 8
	                       | static_cast<uint32_t>(buffer[1]) << 16
	                       | static_cast<uint32_t>(buffer[0]) << 24;
	return value;
}

int32_t ByteArray::readSignedInt()
{
	const uint8_t buffer[4] = {bytes[bytePtr++], bytes[bytePtr++], bytes[bytePtr++], bytes[bytePtr++]};
	const int32_t value = static_cast<int32_t>(buffer[3])
	                      | static_cast<int32_t>(buffer[2]) << 8
	                      | static_cast<int32_t>(buffer[1]) << 16
	                      | static_cast<int32_t>(buffer[0]) << 24;
	return value;
}

void ByteArray::readBytes(uint8_t* target, const std::size_t size)
{
	memcpy(target, &bytes[bytePtr], sizeof(uint8_t) * size);
	bytePtr += size;
}

void ByteArray::setPtr(const std::size_t ptr)
{
	this->bytePtr = ptr;
}

std::size_t ByteArray::getPtr() const {
	return bytePtr - 1;
}

bool ByteArray::atEnd() const {
	return (bytePtr == size);
}

std::size_t ByteArray::getSize() const {
	return size;
}
