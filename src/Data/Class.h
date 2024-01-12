#pragma once

#include "Core.h"

#include "Attribute.h"
#include "Memory.h"
#include "Data/Variable.h"

#include <span>

class FieldInfo {
public:
	uint16_t accessFlags;
	uint16_t nameIndex;
	uint16_t descriptorIndex;
	bool isPrivate;
	AttributeCollection* attributes;
	Variable* staticData;

	[[nodiscard]] bool isStatic() const {
		return ((accessFlags & ACC_STATIC) != 0);
	}
};

class MethodInfo {
public:
	uint16_t accessFlags;
	uint16_t nameIndex;
	uint16_t descriptorIndex;
	AttributeCode* code;
	AttributeCollection* attributes;
	char* returnType;
	char** args;
	uint16_t argsCount;
	const char* name;
public:
	[[nodiscard]] bool isStatic() const {
		return ((accessFlags & ACC_STATIC) != 0);
	}

	[[nodiscard]] bool isPublic() const {
		return ((accessFlags & ACC_PUBLIC) != 0);
	}

	[[nodiscard]] bool isNative() const {
		return ((accessFlags & ACC_NATIVE) != 0);
	}

	[[nodiscard]] bool isConstructor() const {
		return name != nullptr && (strcmp(name, "<init>") == 0);
	}

	[[nodiscard]] bool isAbstract() const {
		return ((accessFlags & ACC_ABSTRACT) != 0);
	}
};

class ClassInfo {
public:
	char* filePath;
	uint64_t size;
	char lastModifiedString[50];
	uint16_t minorVersion;
	uint16_t majorVersion;
	ConstantPool* constantPool;
	uint16_t accessFlags;
	uint16_t thisClass;
	uint16_t superClass;

	uint16_t* interfaces;
	uint16_t interfacesCount;

	FieldInfo** fields;
	uint16_t fieldsCount;

	MethodInfo** methods;
	uint16_t methodCount;

	AttributeCollection* attributes;
	char* sourceFile;
	Memory* memory;
	// Runtime data
	std::span<Variable> staticFields; // An array of data for all static fields, the field also has a copy of this data (could be 1 or 2 items)
	uint16_t staticFieldsCount; // Total amount of static fields
public:
	[[nodiscard]] bool isPublic() const {
		return ((accessFlags & ACC_PUBLIC) != 0);
	}

	[[nodiscard]] MethodInfo* findMethodWithNameAndDescriptor(const char* name, const char* descriptor) const
	{
		for (uint16_t currentMethod = 0; currentMethod < methodCount; ++currentMethod) {
			if (strcmp(methods[currentMethod]->name, name) == 0
				&& strcmp(constantPool->getString(methods[currentMethod]->descriptorIndex), descriptor) == 0)
			{
				return methods[currentMethod];
			}
		}
		return nullptr;
	}

	[[nodiscard]] char* getName() const
	{
		const CPClassInfo* classInfo = constantPool->getClassInfo(thisClass);
		return constantPool->getString(classInfo->nameIndex);
	}

	[[nodiscard]] FieldInfo* findField(char* name, char* descriptor) const
	{
		FieldInfo* field = nullptr;

		for (u2 currentField = 0; currentField < fieldsCount; ++currentField)
		{
			if (strcmp(constantPool->getString(fields[currentField]->nameIndex), name) == 0 &&
			strcmp(constantPool->getString(fields[currentField]->descriptorIndex), descriptor) == 0)
			{
				return fields[currentField];
			}
		}

		return field;
	}
};