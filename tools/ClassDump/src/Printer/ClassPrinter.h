#pragma once

#include "Opcode.h"
#include "ClassLoader/ClassLoader.h"
#include "CodePrinter.h"

class ClassPrinter {
private:
	const char* ACC_PUBLIC_KEYWORD = "public";
	const char* ACC_PRIVATE_KEYWORD = "private";
	const char* ACC_PROTECTED_KEYWORD = "protected";
	const char* ACC_STATIC_KEYWORD = "static";
	const char* ACC_FINAL_KEYWORD = "final";
	const char* ACC_NATIVE_KEYWORD = "native";
	const char* ACC_INTERFACE_KEYWORD = "interface";
	const char* ACC_ABSTRACT_KEYWORD = "abstract";
	const char* ACC_SYNCHRONIZED_KEYWORD = "synchronized";
	const char* ACC_VOLATILE_KEYWORD = "volatile";
	const char* ACC_TRANSIENT_KEYWORD = "transient";

	const char* Unknown = "(unknown)";
	const char* ACC_PUBLIC_STR = "ACC_PUBLIC";
	const char* ACC_PRIVATE_STR = "ACC_PRIVATE";
	const char* ACC_PROTECTED_STR = "ACC_PROTECTED";
	const char* ACC_STATIC_STR = "ACC_STATIC";
	const char* ACC_FINAL_STR = "ACC_FINAL";
	const char* ACC_SUPER_STR = "ACC_SUPER";
	const char* ACC_BRIDGE_STR = "ACC_BRIDGE";
	const char* ACC_VARARGS_STR = "ACC_VARARGS";
	const char* ACC_NATIVE_STR = "ACC_NATIVE";
	const char* ACC_INTERFACE_STR = "ACC_INTERFACE";
	const char* ACC_ABSTRACT_STR = "ACC_ABSTRACT";
	const char* ACC_STRICT_STR = "ACC_STRICT";
	const char* ACC_SYNTHETIC_STR = "ACC_SYNTHETIC";
	const char* ACC_ANNOTATION_STR = "ACC_ANNOTATION";
	const char* ACC_ENUM_STR = "ACC_ENUM";
	const char* ACC_SYNCHRONIZED_STR = "ACC_SYNCHRONIZED";
	const char* ACC_VOLATILE_STR = "ACC_VOLATILE";
	const char* ACC_TRANSIENT_STR = "ACC_TRANSIENT";

	enum AccessFlagType {
		CLASS, METHOD, FIELD
	};

	CodePrinter codePrinter;
	const char* getTypeAsString(ConstantType type) const;
	const char* getTypeAsString(AccessFlag flag, AccessFlagType type) const;
	void getAsExternalReturnType(char* returnType, char* outputBuffer);
	void printField(const FieldInfo* fieldInfo, const ConstantPool* cp, Memory* memory);
	void printMethodSignature(const MethodInfo* methodInfo, 
		const ClassInfo& classInfo, 
		const char* className, 
		const ConstantPool* cp,
		Memory* memory);
	void printMethod(const MethodInfo* methodInfo, const ClassInfo& classInfo, const char* className, const ConstantPool* cp, Memory* memory);
	void printConstantPoolItem(uint16_t currentIndex, const ConstantPool* cp);
	const char* flagToKeyword(AccessFlag flag);
	const char* flagToKeywordClass(AccessFlag flag);
public:
	void printClass(const ClassInfo& classInfo, Memory* memory);
};