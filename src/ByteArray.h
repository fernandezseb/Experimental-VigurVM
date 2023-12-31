#pragma once

#include "Core.h"

#include <span>

class ByteArray {
private:
	std::size_t bytePtr = 0;
	bool allocated = false;
public:
	std::span<u1> bytes;

	ByteArray(u1* bytes, std::size_t size);
	
	u1 readUnsignedByte();
    u2 readUnsignedShort();
	u4 readUnsignedInt();
	i4 readSignedInt();

	void readBytes(u1* target, std::size_t size);
	void setPtr(std::size_t ptr);
	[[nodiscard]] std::size_t getPtr() const;
	[[nodiscard]] bool atEnd() const;
	[[nodiscard]] std::size_t getSize() const;
};