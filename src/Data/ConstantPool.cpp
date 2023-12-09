#include "ConstantPool.h"

void ConstantPool::checkIndex(uint16_t index) const
{
	if (index > this->size) {
		fprintf(stderr, "Error: Invalid index into constant pool: #%" PRIu16 "\n", index);
		Platform::exitProgram(1);
	}
}

char* ConstantPool::getString(uint16_t index) const
{
	checkIndex(index);

	ConstantPoolItem*  item = this->constants[index - 1];
	if (item->getType() != CT_UTF8) {
		fprintf(stderr, "Error: Trying to read UTF8 string at non UTF8 string in constant pool at: #%" PRIu16 "\n", index);
		Platform::exitProgram(1);
	}
	else {
		CPUTF8Info* utf8item = (CPUTF8Info*)item;
		char* chars = (char*) utf8item->bytes;
		return chars;
	}
}

CPClassInfo* ConstantPool::getClassInfo(uint16_t index) const
{
	checkIndex(index);

	ConstantPoolItem* item = this->constants[index - 1];
	if (item->getType() != CT_CLASS) {
		fprintf(stderr, "Error: Trying to read class info at non class info item in constant pool at: #%" PRIu16 "\n", index);
		Platform::exitProgram(1);
	}
	else {
		CPClassInfo* cpClassInfo = (CPClassInfo*)item;
		return cpClassInfo;
	}
}
