#include "AttributePrinter.h"

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

void AttributePrinter::printVerificationList(VerificationTypeInfo* list, uint16_t size, const ConstantPool* cp)
{

	if (size == 0) {
		printf("[]\n");
		return;
	}

	printf("[ ");

	for (uint16_t currentType = 0; currentType < size; ++currentType)
	{
		char typeName[200] = { 0 };
		VerificationTypeInfo info = list[currentType];
		switch (info.tag) {
		case 0: {
			sprintf(typeName,"top");
			break;
		}
		case 1: {
			sprintf(typeName, "int");
			break;
		}
		case 2: {
			sprintf(typeName, "float");
			break;
		}
		case 5: {
			sprintf(typeName, "null");
			break;
		}
		case 6: {
			sprintf(typeName, "uninitialized this");
			break;
		}
		case 7: {
			CPClassInfo* classInfo =  cp->getClassInfo(info.data);
			uint16_t nameIndex =  classInfo->nameIndex;
			char* className = cp->getString(nameIndex);
			sprintf(typeName, "class \"%s\"", className);
			break;
		}
		case 8: {
			sprintf(typeName, "uninitialized(%" PRIu16 ")", info.data);
			break;
		}
		case 4: {
			sprintf(typeName, "long");
			break;
		}
		case 3: {
			sprintf(typeName, "double");
			break;
		}
		}

		if (currentType != 0)
		{
			printf(", ");
		}
		printf("%s", typeName);
	}

	printf(" ]\n");
}

void AttributePrinter::printAttribute(AttributeInfo* attribute, const ConstantPool* cp)
{
	if (attribute->type == SourceFile) {
		AttributeSourceFile* att = (AttributeSourceFile*)attribute;
		printf("SourceFile: \"%s\"\n", cp->getString(att->sourceFileIndex));
	}
	else if (attribute->type == LineNumberTable) {
		AttributeLineNumberTable* att = (AttributeLineNumberTable*)attribute;
		for (uint16_t currentIndex = 0; currentIndex < att->size; ++currentIndex) {
			LineNumberTableEntry& entry = att->entries[currentIndex];
			printf("        line %" PRIu16 ": %" PRIu16 "\n", entry.lineNumber, entry.startPc);
		}
	}
	else if (attribute->type == LocalVariableTable) {
		AttributeLocalVariableTable* att = (AttributeLocalVariableTable*)attribute;
		printf("        Start  Length  Slot  Name   Signature\n");

		for (uint16_t currentIndex = 0; currentIndex < att->size; ++currentIndex) {
			LocalVariableTableEntry& entry = att->entries[currentIndex];
			
			printf("        %5" PRIu16, entry.startPc);
			printf("  %6" PRIu16, entry.length);
			printf("  %4" PRIu16, entry.index);
			printf("  %4s", cp->getString(entry.nameIndex));
			printf("   %-10s\n", cp->getString(entry.descriptorIndex));
		}
	}
	else if (attribute->type == StackMapTable) {
		StackMapTableAttribute* att = (StackMapTableAttribute*)attribute;
		uint8_t indentSize = 8;
		printf("%*cnumber_of_entries = %" PRIu16"\n", indentSize, ' ', att->entriesCount);
		for (uint16_t currentFrame = 0; currentFrame < att->entriesCount; ++currentFrame) {
			switch (att->entries[currentFrame]->frameType) {
			case FullFrameType: {
				FullFrame* frame = (FullFrame*)att->entries[currentFrame];
				printf("%*cframe_type = 255 /* full_frame */\n", indentSize+4, ' ');
				printf("%*coffset_delta = %" PRIu16 "\n", frame->offsetDelta, indentSize + 4, ' ');
				printf("%*clocals = ", indentSize + 4, ' ');
				printVerificationList(frame->locals, frame->numberOfLocals, cp);
				printf("%*cstack = ", indentSize + 4, ' ');
				printVerificationList(frame->stack, frame->numberOfStackItems, cp);
				break;
			}
			case SameFrameType: {
				SameFrame* frame = (SameFrame*)att->entries[currentFrame];
				printf("%*cframe_type = %" PRIu8 " /* same */\n", indentSize + 2, ' ', frame->offsetDelta);
				break;
			}
			case SameLocals1StackItemFrameType: {
				SameLocals1StackItemFrame* frame = (SameLocals1StackItemFrame*)att->entries[currentFrame];
				printf("%*cframe_type = %" PRIu8 " /* same_locals_1_stack_item_frame */\n", indentSize + 2, ' ', frame->offsetDelta);
				printf("%*cstack = ", indentSize + 4, ' ');
				printVerificationList(&frame->stack, 1, cp);
				break;
			}
			case SameLocals1StackItemFrameExtendedType: {
				SameLocals1StackItemFrame* frame = (SameLocals1StackItemFrame*)att->entries[currentFrame];
				printf("%*cframe_type = %" PRIu8 " /* same_locals_1_stack_item_extended_frame */\n", indentSize + 2, ' ', frame->offsetDelta);
				printf("%*cstack = ", indentSize + 4, ' ');
				printVerificationList(&frame->stack, 1, cp);
				break;
			}
			case ChopFrameType: {
				ChopFrame* frame = (ChopFrame*)att->entries[currentFrame];
				printf("%*cframe_type = %" PRIu8 " /* chop_frame */\n", indentSize + 2, ' ', frame->offsetDelta);
				// TODO: Also print the K value?
				break;
			}
			case SameFrameExtendedType: {
				SameFrameExtended* frame = (SameFrameExtended*)att->entries[currentFrame];
				printf("%*cframe_type = %" PRIu8 " /* same_frame_extended */\n", indentSize + 2, ' ', frame->offsetDelta);
				break;
			}
			case AppendFrameType: {
				AppendFrame* frame = (AppendFrame*)att->entries[currentFrame];
				printf("%*cframe_type = %" PRIu8 " /* append_frame */\n", indentSize + 2, ' ', frame->offsetDelta);
				// TODO: Also print the k value?
				break;
			}
			default: {
				fprintf(stderr, "Error: Unknown frame type detected.\n");
				Platform::exitProgram(-5);
			}
			}
		}
	}
	else if (attribute->type == Exceptions) {
		ExceptionsAttribute* att = (ExceptionsAttribute*)attribute;
		uint8_t indentSize = 4;
		printf("%*cExceptions:\n", indentSize, ' ');
		for (uint16_t currentException = 0; currentException < att->exceptionsCount; ++currentException) {
			char* className = cp->getString(cp->getClassInfo(att->exceptionsIndexTable[currentException])->nameIndex);
			char buffer[100] = { 0 };
			binaryClassToExternalCopy(className, buffer);
			printf("%*cthrows %s\n", indentSize+2, ' ', buffer);
		}
	}
}
