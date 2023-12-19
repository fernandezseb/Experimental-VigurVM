#include "ClassLoader/DescriptorParser.h"

#include <stdlib.h>

void assertString(char* testString, const char* expected)
{
	if (strcmp(testString, expected) != 0) {
		fprintf(stderr, "Error: Expected %s, but was %s\n", expected, testString);
		exit(-2);
	}
}

void assertUint16(uint16_t testString, uint16_t expected)
{
	if (testString != expected) {
		fprintf(stderr, "Error: Expected %d, but was %d\n", expected, testString);
		exit(-2);
	}
}

int test1() {
	const char* descriptor = "([Ljava/lang/String;)V";

	uint16_t size;
	char* argsPart = DescriptorParser::getArgsPart((char*)descriptor, &size);
	assertString(argsPart,"[Ljava/lang/String;)V");
	assertUint16(size, 19);


	char* returnPart = DescriptorParser::getReturnPart((char*)descriptor, &size);
	assertString(returnPart, "V");
	assertUint16(size, 1);

	return 0;
}

int test2() {
	const char* descriptor = "([Ljava/lang/String;I[SSI)[[I";

	uint16_t size;
	char* argsPart = DescriptorParser::getArgsPart((char*)descriptor, &size);
	assertString(argsPart, "[Ljava/lang/String;I[SSI)[[I");
	assertUint16(size, 24);

	char* returnPart = DescriptorParser::getReturnPart((char*)descriptor, &size);
	assertString(returnPart, "[[I");
	assertUint16(size, 3);

	return 0;
}

int main(int argc, char* argv[])
{
	Memory memory(4096, 4096);
	int ret = test1();

	if (ret != 0) {
		return ret;
	}

	ret = test2();

	if (ret != 0) {
		return ret;
	}

	uint16_t size;
	char** parts = DescriptorParser::getTypes("[Ljava/lang/String;I[SS[[I",26, &size, &memory);
	if (size != 5) {
		return -2;
	}

	assertString(parts[0], "[Ljava/lang/String;");
	assertString(parts[1], "I");
	assertString(parts[2], "[S");
	assertString(parts[3], "S");
	assertString(parts[4], "[[I");

	parts = DescriptorParser::getTypes("I",1, &size, &memory);
	if (size != 1) {
		return -2;
	}

	assertString(parts[0], "I");

	return 0;
}