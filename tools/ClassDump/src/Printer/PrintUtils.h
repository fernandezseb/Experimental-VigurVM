#pragma once

#include "Data/ConstantPool.h"
#include "Util.h"

class PrintUtils {
public:
	static char* printResolvedInline(char* buffer, const ConstantPoolItem* item, const ConstantPool* cp);
	static char* printData(char* buffer, const ConstantPoolItem* item, const ConstantPool* cp);
	static char* printResolved(char* buffer, const ConstantPoolItem* item, const ConstantPool* cp);
};