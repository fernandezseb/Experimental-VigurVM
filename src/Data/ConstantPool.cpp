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

#include "ConstantPool.h"

#include <cstdlib>

void ConstantPool::checkIndex(uint16_t index) const
{
	if (index > this->constants.size()) {
		fprintf(stderr, "Error: Invalid index into constant pool: #%" PRIu16 "\n", index);
		Platform::exitProgram(1);
	}
}

std::string_view ConstantPool::getString(uint16_t index) const
{
	checkIndex(index);

	const ConstantPoolItem*  item = this->constants[index - 1];
	if (item->getType() != CT_UTF8) {
		fprintf(stderr, "Error: Trying to read UTF8 string at non UTF8 string in constant pool at: #%" PRIu16 "\n", index);
		Platform::exitProgram(1);
	}
	else {
		const CPUTF8Info* utf8item = static_cast<const CPUTF8Info*>(item);
		return {reinterpret_cast<char*>(utf8item->bytes)};
	}
	return {};
}

CPClassInfo* ConstantPool::getClassInfo(uint16_t index) const
{
	checkIndex(index);

	ConstantPoolItem* item = this->constants[index - 1];
	if (item->getType() != CT_CLASS) [[unlikely]] {
		fprintf(stderr, "Error: Trying to read class info at non class info item in constant pool at: #%" PRIu16 "\n", index);
		Platform::exitProgram(1);
	}
	else [[likely]] {
		CPClassInfo* cpClassInfo = (CPClassInfo*)item;
		return cpClassInfo;
	}

	return nullptr;
}

CPMethodRef* ConstantPool::getMethodRef(u2 index) const
{
	checkIndex(index);

	ConstantPoolItem* item = this->constants[index - 1];
	if (item->getType() != CT_METHODREF) {
		fprintf(stderr, "Error: Trying to read method ref at non method ref item in constant pool at: #%" PRIu16 "\n", index);
		Platform::exitProgram(1);
	}
	else {
		CPMethodRef* cpMethodRef = (CPMethodRef*)item;
		return cpMethodRef;
	}
	return nullptr;
}

CPNameAndTypeInfo* ConstantPool::getNameAndTypeInfo(u2 index) const
{
	checkIndex(index);

	ConstantPoolItem* item = this->constants[index - 1];
	if (item->getType() != CT_NAMEANDTYPE) {
		fprintf(stderr, "Error: Trying to read name and type at non name and type item in constant pool at: #%" PRIu16 "\n", index);
		Platform::exitProgram(1);
	}
	else {
		CPNameAndTypeInfo* cpNameAndType = (CPNameAndTypeInfo*)item;
		return cpNameAndType;
	}
	return nullptr;
}

CPFieldRef* ConstantPool::getFieldRef(uint16_t index) const
{
	checkIndex(index);

	ConstantPoolItem* item = this->constants[index - 1];
	if (item->getType() != CT_FIELDREF) {
		fprintf(stderr, "Error: Trying to read field ref at non field ref item in constant pool at: #%" PRIu16 "\n", index);
		Platform::exitProgram(1);
	}
	else {
		CPFieldRef* cpFieldRef = (CPFieldRef*)item;
		return cpFieldRef;
	}
	return nullptr;
}

CPInterfaceRef * ConstantPool::getInterfaceMethodRef(const u2 index) const {
	checkIndex(index);

	ConstantPoolItem* item = this->constants[index - 1];
	if (item->getType() != CT_INTERFACEMETHOD) {
		fprintf(stderr, "Error: Trying to read interface method ref at non field interface method item in constant pool at: #%" PRIu16 "\n", index);
		exit(1);
	}

	return static_cast<CPInterfaceRef*>(item);
}
