#include "CodePrinter.h"

#include "PrintUtils.h"

void SignedBytePrinter(uint8_t* args, uint16_t argsCount, const ConstantPool* cp)
{
	for (uint16_t currentIndex = 0; currentIndex < argsCount; ++currentIndex) {
		int8_t signedInt = *reinterpret_cast<int8_t*>(&args[currentIndex]);
		printf(" %" PRIi8, signedInt);
	}
}

void UnsignedBytePrinter(uint8_t* args, uint16_t argsCount, const ConstantPool* cp)
{
	for (uint16_t currentIndex = 0; currentIndex < argsCount; ++currentIndex) {
		printf(" %" PRIu8, args[currentIndex]);
	}
}

void SignedShort(uint8_t* args, uint16_t argsCount, const ConstantPool* cp)
{
	for (int i = 0; i < argsCount; i += 2) {
		uint8_t byte1 = args[i];
		uint8_t byte2 = args[i + 1];

		int16_t shortIndex = (byte1 << 8) | byte2;

		printf(" %" PRIi16, shortIndex);
	}
}

void ShortIndices(uint8_t* args, uint16_t argsCount, const ConstantPool* cp)
{
	for (int i = 0; i < argsCount; i += 2) {
		uint8_t byte1 = args[i];
		uint8_t byte2 = args[i + 1];

		uint16_t shortIndex = (byte1 << 8) | byte2;

		char indexStr[6];
		sprintf(indexStr, " #%" PRIu16, shortIndex);
		printf("%-25s", indexStr);
	}

	if (argsCount > 0) {
		printf("//");
	}
	for (int i = 0; i < argsCount; i += 2) {
		uint8_t byte1 = args[i];
		uint8_t byte2 = args[i + 1];

		uint16_t shortIndex = (byte1 << 8) | byte2;
		if (shortIndex <= 0) {
			fprintf(stderr, "Invalid index in CP\n");
			Platform::exitProgram(1);
		}
		ConstantPoolItem* item = cp->constants[shortIndex - 1];
		if (item != 0) {
			char buffer[300] = {0};
			PrintUtils::printResolvedInline(buffer, cp->constants[shortIndex - 1], cp);
			jprintf(" %s", buffer);
		}
	}
	jflush();
}

void InvokeDynamicPrinter(uint8_t* args, uint16_t argsCount, const ConstantPool* cp)
{
	uint8_t byte1 = args[0];
	uint8_t byte2 = args[1];

	uint16_t shortIndex = (byte1 << 8) | byte2;

	char indexStr[6];
	sprintf(indexStr, " #%" PRIu16, shortIndex);
	printf("%-25s", indexStr);
}

void ByteIndices(uint8_t* args, uint16_t argsCount, const ConstantPool* cp)
{
	for (int i = 0; i < argsCount; ++i) {
		uint8_t byte = args[i];
		char indexStr[6];

		sprintf(indexStr, " #%" PRIu8, byte);
		printf("%-25s", indexStr);
	}

	if (argsCount > 0) {
		printf("//");
	}
	for (int i = 0; i < argsCount; ++i) {
		uint8_t byte = args[i];

		ConstantPoolItem* item = cp->constants[byte-1];
		if (item != 0) {
			char buffer[300] = { 0 };
			PrintUtils::printResolvedInline(buffer, cp->constants[byte - 1], cp);
			jprintf(" %s", buffer);
		}
	}
	jflush();
}

void ArrayTypePrinter(uint8_t* args, uint16_t argsCount, const ConstantPool* cp)
{
	const char* type;
	uint8_t arg = args[0];
	switch (arg) {
	case 4:
		type = T_BOOLEAN;
		break;
	case 5:
		type = T_CHAR;
		break;
	case 6:
		type = T_FLOAT;
		break;
	case 7:
		type = T_DOUBLE;
		break;
	case 8:
		type = T_BYTE;
		break;
	case 9:
		type = T_SHORT;
		break;
	case 10:
		type = T_INT;
		break;
	case 11:
		type = T_LONG;
		break;
	default:
		type = "(unknown)";
		break;
	}

	printf(" %s", type);
}

void MultiArrayPrinter(uint8_t* args, uint16_t argsCount, const ConstantPool* cp)
{
	uint8_t byte1 = args[0];
	uint8_t byte2 = args[1];
	uint8_t dimensions = args[2];
	uint16_t shortIndex = (byte1 << 8) | byte2;
	printf(" #%" PRIu16 ", %" PRIu8, shortIndex, dimensions);
}

void CodePrinter::printCode(
	const AttributeCode* code,
	const MethodInfo* method,
	const ConstantPool* cp,
	Memory* memory
)
{
	int argsSize = method->argsCount;
	if (!method->isStatic()) {
		argsSize++;
	}

	printf("      stack=%" PRIu16 ", locals=%" PRIu16 ", args_size=%" PRIu16 "\n",
		code->maxStack, code->maxLocals, argsSize);

	ByteArray byteArray(code->code, code->codeLength);

	while (!byteArray.atEnd()) {
		uint8_t opcode = byteArray.readUnsignedByte();
		bool found = false;
		for (const Instruction& instruction : this->instructions) {
			if (((uint8_t)instruction.opcode) == opcode) {
				char indexStr[6];
				sprintf(indexStr, "%" PRIu64 ": ", byteArray.getPtr());
				printf("%12s%-13s", indexStr, instruction.name);
				uint8_t* args = 0;
				if (instruction.args > 0) {
					args = (uint8_t*)memory->alloc(instruction.args);
					byteArray.readBytes(args, instruction.args);
				}
				if (instruction.printFunction != NULL) {
					instruction.printFunction(args, instruction.args, cp);
				}

				found = true;
				break;
			}
		}
		if (!found) {
			if (opcode == 0xab) {

				uint32_t instructionIndex = byteArray.getPtr();
				// Next byte is 4 byte aligned
				while ((byteArray.getPtr() + 1) % 4 != 0) {
					byteArray.readUnsignedByte();
				}

				int32_t defaultOffset = byteArray.readSignedInt();
				int32_t defaultAddress = instructionIndex + defaultOffset;
				int32_t nPairs = byteArray.readSignedInt();

				char indexStr[6];
				sprintf(indexStr, "%" PRIu64 ": ", byteArray.getPtr());
				printf("%12s%-13s", indexStr, "lookupswitch");

				printf(" { // %" PRIi32 "\n", nPairs);

				for (int32_t currentPair = 0; currentPair < nPairs; currentPair++) {
					int32_t matchKey = byteArray.readSignedInt();
					int32_t offset = byteArray.readSignedInt();

					printf("%24" PRIi32, matchKey);

					printf(": %" PRIi32, (instructionIndex + offset));
					printf("\n");
				}

				printf("%24s", "default");
				printf(": %" PRIi32 "\n", defaultAddress);
				printf("            }");

				found = true;
			}

		}
		if (!found) {
			printf("Error: Instruction with opcode: %" PRIu8 " unrecognized\n", opcode);
			Platform::exitProgram(1);
		}
		printf("\n");
	}

	for (size_t currentAttrib = 0; currentAttrib < code->attributes->attributesCount; ++currentAttrib) {
		AttributeInfo* att = code->attributes->attributes[currentAttrib];
		if (att != 0) {
			printf("      %s:\n", cp->getString(att->attributeNameIndex));
			AttributePrinter::printAttribute(att, cp);
		}
	}

}