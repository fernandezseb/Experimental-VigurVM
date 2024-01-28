#pragma once

#include "Core.h"

#include "Attribute.h"
#include "Memory.h"
#include "Data/Variable.h"

#include <span>
#include <string_view>

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
	std::string_view name;
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
		return name == "<init>";
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

	std::span<MethodInfo*> methods;

	AttributeCollection* attributes;
	std::string_view sourceFile;
	Memory* memory;
	// Runtime data
	std::span<Variable> staticFields; // An array of data for all static fields, the field also has a copy of this data (could be 1 or 2 items)
	uint16_t staticFieldsCount; // Total amount of static fields
public:
	[[nodiscard]] bool isPublic() const {
		return (accessFlags & ACC_PUBLIC) != 0;
	}

	[[nodiscard]] bool isInterface() const
	{
		return (accessFlags & ACC_INTERFACE) != 0;
	}

	[[nodiscard]] bool isArrayType() const
	{
		const std::string_view name = getName();
		return name.starts_with('[');
	}

	[[nodiscard]] MethodInfo* findMethodWithNameAndDescriptor(const char* name, const char* descriptor) const
	{
		for (uint16_t currentMethod = 0; currentMethod < methods.size(); ++currentMethod) {
			if (methods[currentMethod]->name == name
				&& constantPool->getString(methods[currentMethod]->descriptorIndex) ==  descriptor)
			{
				return methods[currentMethod];
			}
		}
		return nullptr;
	}

	[[nodiscard]] std::string_view getName() const
	{
		const CPClassInfo* classInfo = constantPool->getClassInfo(thisClass);
		return constantPool->getString(classInfo->nameIndex);
	}

	[[nodiscard]] FieldInfo* findField(const char* name, const char* descriptor) const
	{
		FieldInfo* field = nullptr;

		for (u2 currentField = 0; currentField < fieldsCount; ++currentField)
		{
			if (constantPool->getString(fields[currentField]->nameIndex) == name &&
			constantPool->getString(fields[currentField]->descriptorIndex) == descriptor)
			{
				return fields[currentField];
			}
		}

		return field;
	}
};