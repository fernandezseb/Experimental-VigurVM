#pragma once

#include "Core.h"

#include "Attribute.h"
#include "../Memory.h"
#include "Data/Variable.h"

class FieldInfo {
public:
	uint16_t accessFlags;
	uint16_t nameIndex;
	uint16_t descriptorIndex;
	bool isPrivate;
	AttributeCollection* attributes;
	bool isStatic() const {
		return ((accessFlags & ACC_STATIC) != 0);
	}
	Variable* staticData;
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
	bool isStatic() const {
		return ((accessFlags & ACC_STATIC) != 0);
	}

	bool isPublic() const {
		return ((accessFlags & ACC_PUBLIC) != 0);
	}

	bool isNative() const {
		return ((accessFlags & ACC_NATIVE) != 0);
	}

	bool isConstructor() const {
		return (strcmp(name, "<init>") == 0);
	}

	bool isAbstract() const {
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
	Variable* staticFields;
	uint16_t staticFieldsCount;
public:
	bool isPublic() const {
		return ((accessFlags & ACC_PUBLIC) != 0);
	}

	MethodInfo* findMethodWithName(const char* name) const
	{
		for (uint16_t currentMethod = 0; currentMethod < methodCount; ++currentMethod) {
			if (strcmp(methods[currentMethod]->name, name) == 0)
			{
				return methods[currentMethod];
			}
		}
		return nullptr;
	}

	[[nodiscard]] char* getName() const
	{
		CPClassInfo* classInfo = constantPool->getClassInfo(thisClass);
		return constantPool->getString(classInfo->nameIndex);
	}
};