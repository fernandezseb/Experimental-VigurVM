#include "ClassPrinter.h"

#include "AttributePrinter.h"

#include "Util.h"
#include "PrintUtils.h"

const char* ClassPrinter::getTypeAsString(ConstantType type) const {
	switch (type) {
	case CT_UTF8:
	{
		return "Utf8";
	}
	case CT_INTEGER:
	{
		return "Integer";
	}
	case CT_FLOAT:
	{
		return "Float";
	}
	case CT_LONG:
	{
		return "Long";
	}
	case CT_DOUBLE:
	{
		return "Double";
	}
	case CT_CLASS:
	{
		return "Class";
	}
	case CT_STRING:
	{
		return "String";
	}
	case CT_FIELDREF:
	{
		return "Fieldref";
	}
	case CT_METHODREF:
	{
		return "Methodref";
	}
	case CT_INTERFACEMETHOD:
	{
		return "Interfacemethod";
	}
	case CT_NAMEANDTYPE:
	{
		return "NameAndType";
	}
	case CT_METHODHANDLE:
	{
		return "MethodHandle";
	}
	case CT_METHODTYPE:
	{
		return "Methodtype";
	}
	case CT_INVOKEDYNAMIC:
	{
		return "InvokeDynamic";
	}
	default:
	{
		return "Unknown";
	}
	}
}

const char* ClassPrinter::getTypeAsString(AccessFlag flag, AccessFlagType type) const
{
	switch (type) {
	case METHOD:
	{
		switch (flag) {
		case ACC_PUBLIC:
			return ACC_PUBLIC_STR;
		case ACC_PRIVATE:
			return ACC_PRIVATE_STR;
		case ACC_PROTECTED:
			return ACC_PROTECTED_STR;
		case ACC_STATIC:
			return ACC_STATIC_STR;
		case ACC_FINAL:
			return ACC_FINAL_STR;
		case ACC_SYNCHRONIZED:
			return ACC_SYNCHRONIZED_STR;
		case ACC_BRIDGE:
			return ACC_BRIDGE_STR;
		case ACC_VARARGS:
			return ACC_VARARGS_STR;
		case ACC_NATIVE:
			return ACC_NATIVE_STR;
		case ACC_ABSTRACT:
			return ACC_ABSTRACT_STR;
		case ACC_STRICT:
			return ACC_STRICT_STR;
		case ACC_SYNTHETIC:
			return ACC_SYNTHETIC_STR;
		}
	}
	case FIELD:
	{
		switch (flag) {
		case ACC_PUBLIC:
			return ACC_PUBLIC_STR;
		case ACC_PRIVATE:
			return ACC_PRIVATE_STR;
		case ACC_PROTECTED:
			return ACC_PROTECTED_STR;
		case ACC_STATIC:
			return ACC_STATIC_STR;
		case ACC_FINAL:
			return ACC_FINAL_STR;
		case ACC_VOLATILE:
			return ACC_VOLATILE_STR;
		case ACC_TRANSIENT:
			return ACC_TRANSIENT_STR;
		case ACC_SYNTHETIC:
			return ACC_SYNTHETIC_STR;
		case ACC_ENUM:
			return ACC_ENUM_STR;
		}
	}
	case CLASS:
	{
		switch (flag) {
		case ACC_PUBLIC:
			return ACC_PUBLIC_STR;
		case ACC_FINAL:
			return ACC_FINAL_STR;
		case ACC_SUPER:
			return ACC_SUPER_STR;
		case ACC_INTERFACE:
			return ACC_INTERFACE_STR;
		case ACC_ABSTRACT:
			return ACC_ABSTRACT_STR;
		case ACC_SYNTHETIC:
			return ACC_SYNTHETIC_STR;
		case ACC_ANNOTATION:
			return ACC_ANNOTATION_STR;
		case ACC_ENUM:
			return ACC_ENUM_STR;
		}
	}
	}
}

inline const char* charToType(char c) {
	switch (c) {
	case 'V': {
		return T_VOID;
	}
	case 'B': {
		return T_BYTE;
	}
	case 'C': {
		return T_CHAR;
	}
	case 'D': {
		return T_DOUBLE;
	}
	case 'F': {
		return T_FLOAT;
	}
	case 'I': {
		return T_INT;
	}
	case 'J': {
		return T_LONG;
	}
	case 'S': {
		return T_SHORT;
	}
	case 'Z': {
		return T_BOOLEAN;
	}
	default: {
		return "Unknown";
	}
	}
}

inline const char* ClassPrinter::flagToKeyword(AccessFlag flag) {
	switch (flag) {
	case ACC_PUBLIC:
		return ACC_PUBLIC_KEYWORD;
	case ACC_PRIVATE:
		return ACC_PRIVATE_KEYWORD;
	case ACC_PROTECTED:
		return ACC_PROTECTED_KEYWORD;
	case ACC_STATIC:
		return ACC_STATIC_KEYWORD;
	case ACC_FINAL:
		return ACC_FINAL_KEYWORD;
	case ACC_NATIVE:
		return ACC_NATIVE_KEYWORD;
	case ACC_ABSTRACT:
		return ACC_ABSTRACT_KEYWORD;
	case ACC_SYNCHRONIZED:
		return ACC_SYNCHRONIZED_KEYWORD;
	case ACC_VOLATILE:
		return ACC_VOLATILE_KEYWORD;
	case ACC_TRANSIENT:
		return ACC_TRANSIENT_KEYWORD;
	case ACC_SYNTHETIC:
		return "";
	}
}

inline const char* ClassPrinter::flagToKeywordClass(AccessFlag flag) {
	switch (flag) {
	case ACC_PUBLIC:
		return ACC_PUBLIC_KEYWORD;
	case ACC_PRIVATE:
		return ACC_PRIVATE_KEYWORD;
	case ACC_PROTECTED:
		return ACC_PROTECTED_KEYWORD;
	case ACC_STATIC:
		return ACC_STATIC_KEYWORD;
	case ACC_FINAL:
		return ACC_FINAL_KEYWORD;
	case ACC_ABSTRACT:
		return ACC_ABSTRACT_KEYWORD;
	default:
		return nullptr;
	}
}

void ClassPrinter::getAsExternalReturnType(char* returnType, char* buffer)
{
	int arrCount = 0;
	bool inClass = false;

	for (uint16_t currentChar = 0; returnType[currentChar] != 0; ++currentChar) {
		char c = returnType[currentChar];
		if (c == 'L') {
			inClass = true;
		}
		else if (c == ';') {
			inClass = false;
		}
		else if (inClass) {
			if (c == '/') {
				strcat(buffer, ".");
			}
			else {
				char str[2] = { c, 0 };
				strcat(buffer, str);
			}
		}
		else if (c == '[') {
			arrCount++;
		}
		else {
			strcat(buffer, charToType(c));
		}
	}

	for (int curr = 0; curr < arrCount; curr++) {
		strcat(buffer, "[]");
	}
}

void ClassPrinter::printField(const FieldInfo* fieldInfo, const ConstantPool* cp, Memory* memory)
{
	const char* descriptor = cp->getString(fieldInfo->descriptorIndex);
	const char* name = cp->getString(fieldInfo->nameIndex);
	printf("  ");

	AccessFlag accessFlagsField[] = { 
		ACC_PUBLIC, 
		ACC_PRIVATE, 
		ACC_PROTECTED, 
		ACC_STATIC, 
		ACC_FINAL, 
		ACC_VOLATILE, 
		ACC_TRANSIENT, 
		ACC_SYNTHETIC, 
		ACC_ENUM
	};

	for (auto const& flag : accessFlagsField) {
		if ((flag & fieldInfo->accessFlags) != 0) {
				jprintf("%s ", flagToKeyword(flag));
		}
	}
	jflush();

	char bufferType[300] = { 0 };
	getAsExternalReturnType((char*)descriptor, bufferType);
	printf("%s %s;\n", bufferType, name);
	printf("    descriptor: %s\n", descriptor);
	printf("    flags: ");

	const char** flags = (const char**)memory->alloc(9 * sizeof(char*));
	int currentIndex = 0;
	for (auto const& flag : accessFlagsField) {
		if ((flag & fieldInfo->accessFlags) != 0) {
			flags[currentIndex] = getTypeAsString(flag, FIELD);
			++currentIndex;
		}
	}
	char buffer[300];
	joinStrings((char**)flags, currentIndex, ", ", 300, buffer);
	printf("%s\n", buffer);

	for (uint16_t currentAttribute = 0; currentAttribute < fieldInfo->attributes->attributesCount; ++currentAttribute)
	{
		uint16_t nameIndex = fieldInfo->attributes->attributes[currentAttribute]->attributeNameIndex;
		char* name = cp->getString(nameIndex);
		
		if (strcmp(name, "ConstantValue") == 0) {
			AttributeConstantValue* att = (AttributeConstantValue*)fieldInfo->attributes->attributes[currentAttribute];
			char ctBuffer[100] = {0};
			PrintUtils::printResolvedInline(ctBuffer, cp->constants[att->constantValueIndex - 1], cp);
			printf("    ConstantValue: %s\n", ctBuffer);
			break;
		}
	}

	printf("\n");
}

void ClassPrinter::printMethodSignature(
	const MethodInfo* methodInfo,
	const ClassInfo& classInfo,
	const char* className,
	const ConstantPool* cp,
	Memory* memory)
{
	if (!methodInfo->isConstructor()) {
		char typeBuffer[300] = {0};
		getAsExternalReturnType(methodInfo->returnType, typeBuffer);
		printf("%s ", typeBuffer);
	}

	if (methodInfo->isConstructor()) {
		printf("%s",className);
	}
	else
	{
		printf("%s", cp->getString(methodInfo->nameIndex));
	}
	printf("(");

	char** args = (char**) memory->alloc(sizeof(char*) * methodInfo->argsCount);
	for (int currentArg = 0; currentArg < methodInfo->argsCount; ++currentArg) {
		const char* arg = methodInfo->args[currentArg];
		char typeBuffer[300] = { 0 };
		getAsExternalReturnType((char*)arg, typeBuffer);
		size_t size = strlen(typeBuffer) + 1;
		char* argPtr = (char*) memory->alloc(strlen(typeBuffer) + 1);
		memcpy(argPtr, typeBuffer, size);
		args[currentArg] = argPtr;
	}

	char output[300];
	joinStrings(
		args, methodInfo->argsCount, ",", 200, output);
	printf("%s", output);

	printf(");\n"); // TODO: Add printing of checked exceptions
}

void ClassPrinter::printMethod(const MethodInfo* methodInfo, const ClassInfo& classInfo, const char* className, const ConstantPool* cp, Memory* memory)
{
	printf("  ");

	AccessFlag accessFlagsMethod[] = { 
		ACC_PUBLIC,
		ACC_PRIVATE,
		ACC_PROTECTED,
		ACC_STATIC,
		ACC_FINAL,
		ACC_SYNCHRONIZED,
		ACC_BRIDGE,
		ACC_VARARGS,
		ACC_NATIVE,
		ACC_ABSTRACT,
		ACC_STRICT
	};

	for (auto const& flag : accessFlagsMethod) {
		if ((flag & methodInfo->accessFlags) != 0) {
				printf("%s ", flagToKeyword(flag));
			}
	}


	printMethodSignature(methodInfo, classInfo, className, cp, memory);
	printf("    descriptor: %s\n", cp->getString(methodInfo->descriptorIndex));

	printf("    flags: ");
	const char** flags = (const char**)memory->alloc(50 * sizeof(char*));
	int currentIndex = 0;
	for (auto const& flag : accessFlagsMethod) {
		if ((flag & methodInfo->accessFlags) != 0) {
			flags[currentIndex] = getTypeAsString(flag, METHOD);
			++currentIndex;
		}
	}

	char outBuffer[300];
	joinStrings((char**)flags, currentIndex, ", ", 300, outBuffer);
	printf("%s\n", outBuffer);

	if (methodInfo->isNative() || methodInfo->isAbstract()) {
	}
	else {
		printf("    Code: \n");
		codePrinter.printCode(methodInfo->code, methodInfo, cp, memory);
	}
	for (uint16_t currentAttrib = 0; currentAttrib < methodInfo->attributes->attributesCount; ++currentAttrib) {
		AttributePrinter::printAttribute(methodInfo->attributes->attributes[currentAttrib], cp);
	}
	printf("\n");
}


void ClassPrinter::printConstantPoolItem(uint16_t currentIndex, const ConstantPool* cp) {
	ConstantPoolItem* item = cp->constants[currentIndex];
	if (item == 0) {
		return;
	}
	char indexStr[6];
	sprintf(indexStr, "#%" PRIu16, (currentIndex+1));

	printf("%5s", indexStr);
	char dataBuffer[300] = {0};
	PrintUtils::printData(dataBuffer, item, cp);
	jprintf(" = %-15s%-15s", getTypeAsString(item->getType()), dataBuffer);

	char buffer[300] = { 0 };
	char* out2 = PrintUtils::printResolved(buffer, item, cp);
	if (strlen(out2) > 0) {
		jprintf("// %s", out2);
	}
	jflush();
	printf("\n");
}
static inline void binaryClassToExternalCopy(const char* className, char* output)
{
	size_t size = strlen(className);
	memcpy(output, className, size + 1);
	for (int currentCharacter = 0; currentCharacter < size; ++currentCharacter) {
		if (output[currentCharacter] == '/') {
			output[currentCharacter] = '.';
		}
	}
}

static inline void binaryClassToExternalInPlace(char* output)
{
	size_t size = strlen(output);
	for (int currentCharacter = 0; currentCharacter < size; ++currentCharacter) {
		if (output[currentCharacter] == '/') {
			output[currentCharacter] = '.';
		}
	}
}

void ClassPrinter::printClass(const ClassInfo& classInfo, Memory* memory)
{
	AccessFlag accessFlagsClass[] = {
		ACC_PUBLIC, 
		ACC_FINAL, 
		ACC_SUPER, 
		ACC_INTERFACE,
		ACC_ABSTRACT,
		ACC_SYNTHETIC,
		ACC_ANNOTATION,
		ACC_ENUM
	};

	printf("Classfile %s\n", classInfo.filePath);
	printf("  Last modified %s; size %" PRIu64 " bytes\n", classInfo.lastModifiedString, classInfo.size);
	printf("  MD5 checksum %s\n", classInfo.md5);
	
	if (classInfo.sourceFile != "") {
		printf("  Compiled from \"%s\"\n", classInfo.sourceFile);
	}

	const ConstantPool* const cp = classInfo.constantPool;
	const CPClassInfo* const classPtr = cp->getClassInfo(classInfo.thisClass);
	const CPClassInfo* const superClassPtr = cp->getClassInfo(classInfo.superClass);

	for (auto const& flag : accessFlagsClass) {
		if ((flag & classInfo.accessFlags) != 0) {
			const char* keyword = flagToKeywordClass(flag);
			if (keyword != NULL) {
				printf("%s ", keyword);
			}
		}
	}

	const char* className = cp->getString(classPtr->nameIndex);
	printf("class %s", className);

	const char* superClassName = cp->getString(superClassPtr->nameIndex);
	if (strcmp("java/lang/Object", superClassName) != 0) {
		char* outputBuffer = (char*) Platform::allocateMemory(strlen(superClassName) + 1, 0);
		binaryClassToExternalCopy(superClassName, outputBuffer);
		printf(" extends %s", outputBuffer);
		Platform::freeMemory(outputBuffer);
	}

	if (classInfo.interfacesCount > 0) {
		printf(" implements ");

		char** names = (char**) memory->alloc(sizeof(char*) * classInfo.interfacesCount);

		for (size_t currentIndex = 0; currentIndex < classInfo.interfacesCount; ++currentIndex) {
			uint16_t index = classInfo.interfaces[currentIndex];
			CPClassInfo* classPtr = cp->getClassInfo(index);
			names[currentIndex] = (char*)cp->getString(classPtr->nameIndex);
		}

		char* outputBuffer = (char*)Platform::allocateMemory(300, 0);
		joinStrings(names, classInfo.interfacesCount, ", ", 300, outputBuffer);
		binaryClassToExternalInPlace(outputBuffer);
		printf("%s", outputBuffer);
		Platform::freeMemory(outputBuffer);

	}
	printf("\n");

	printf("  minor version: %" PRIu16 "\n", classInfo.minorVersion);
	printf("  major version: %" PRIu16 "\n", classInfo.majorVersion);
	
	printf("  flags: ");
	const char** flags = (const char**)memory->alloc(8 * sizeof(char*));
	int currentIndex = 0;
	for (auto const& flag : accessFlagsClass) {
		if ((flag & classInfo.accessFlags) != 0) {
			flags[currentIndex] = getTypeAsString(flag, CLASS);
			++currentIndex;
		}
	}
	char buffer[300];
	joinStrings((char**)flags, currentIndex, ", ", 300, buffer);
	printf("%s\n", buffer);

	printf("Constant pool:\n");
	for (uint16_t currentIndex = 0; currentIndex < cp->size; ++currentIndex) {
		printConstantPoolItem(currentIndex, cp);
	}

	printf("{\n");

	// Fields
	for (uint16_t currentField = 0; currentField < classInfo.fieldsCount; ++currentField) {
		FieldInfo* fieldInfo = classInfo.fields[currentField];
		printField(fieldInfo, cp, memory);
	}

	// Methods
	for (uint16_t currentMethod = 0; currentMethod < classInfo.methodCount; ++currentMethod) {
		MethodInfo* methodInfo = classInfo.methods[currentMethod];
		printMethod(methodInfo, classInfo, className, cp, memory);
	}
	
	printf("}\n");

	for (size_t currentAttrib = 0; currentAttrib < classInfo.attributes->attributesCount; ++currentAttrib) {
		AttributeInfo* att = classInfo.attributes->attributes[currentAttrib];
		AttributePrinter::printAttribute(att, cp);
	}
}
