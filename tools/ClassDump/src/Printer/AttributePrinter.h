#pragma once

#include "Data/Attribute.h"
#include "Data/ConstantPool.h"

class AttributePrinter {
private:
	static void printVerificationList(VerificationTypeInfo* list, uint16_t size, const ConstantPool* cp);
public:
	static void printAttribute(AttributeInfo* attribute, const ConstantPool* cp);
};