#pragma once

#include "Core.h"
#include "ByteArray.h"
#include "AttributeParser.h"
#include "Memory.h"
#include "Data/Class.h"
#include "Data/ConstantPool.h"

// MAGIC NUMBER
#define MAGIC_NUMBER       0xCAFEBABE

class ClassLoader {
private:
	void checkMagicNumber(ByteArray& byteArray);
	ConstantPool* readConstantPool(ByteArray& byteArray);
	ConstantPoolItem* readConstantPoolItem(uint8_t tag, ByteArray& byteArray);
	void parseDescriptor(const char* descriptor, MethodInfo* method);
	uint16_t* readInterfaces(ByteArray& byteArray, uint16_t interfacesCount);
	FieldInfo** readFields(ByteArray& byteArray, ConstantPool* constantPool, uint16_t fieldsCount);
	MethodInfo** readMethods(ByteArray& byteArray, ConstantPool* constantPool, uint16_t methodCount);
	ClassInfo* readClass(ByteArray& byteArray);
	Memory* memory;
public:
	ClassInfo* readClass(const char* className, Memory* memory);
};