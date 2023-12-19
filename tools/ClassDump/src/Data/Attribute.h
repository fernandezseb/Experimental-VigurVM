#pragma once

#include "Core.h"

#include "ConstantPool.h"

struct ExceptionTableEntry {
	uint16_t startPc;
	uint16_t endPc;
	uint16_t handlerPc;
	uint16_t catchType;
};

enum AttributeType : uint8_t {
	ConstantValue,
	Code,
	SourceFile,
	LineNumberTable,
	LocalVariableTable,
	LocalVariableTypeTable,
	StackMapTable,
	Exceptions,
	InnerClasses,
	BootstrapMethods,
	Deprecated,
	RuntimeVisibleAnnotations,
	RuntimeInvisibleAnnotations,
	Synthetic,
	Signature,
	AnnotationDefault,
	EnclosingMethod,
	SourceDebugExtension,
	MethodParameters,
	RuntimeVisibleParameterAnnotations,
	RuntimeInvisibleParameterAnnotations,
	RuntimeVisibleTypeAnnotations,
	RuntimeInvisibleTypeAnnotations
};

struct AttributeInfo {
	AttributeType type;
	uint16_t attributeNameIndex;
	uint32_t attributeLength;
};

struct AttributeCollection {
	AttributeInfo** attributes;
	uint16_t attributesCount;

	AttributeInfo* findAttributeWithName(ConstantPool* constantPool, const char* name) const;
};

struct AttributeConstantValue : public AttributeInfo {
	uint16_t constantValueIndex;
};

struct AttributeLineNumberTable;
struct AttributeLocalVariableTable;

struct AttributeCode : public AttributeInfo {
	uint16_t maxStack;
	uint16_t maxLocals;
	uint32_t codeLength;
	uint8_t* code;
	ExceptionTableEntry* exceptionTable;
	uint16_t exceptionTableSize;
	AttributeCollection* attributes;
};

struct AttributeSourceFile : public AttributeInfo {
	uint16_t sourceFileIndex;
};

struct LineNumberTableEntry {
	uint16_t startPc;
	uint16_t lineNumber;
};

struct AttributeLineNumberTable : public AttributeInfo {
	LineNumberTableEntry* entries;
	uint16_t size;
};

struct LocalVariableTableEntry {
	uint16_t startPc;
	uint16_t length;
	uint16_t nameIndex;
	uint16_t descriptorIndex;
	uint16_t index;
};

struct AttributeLocalVariableTable : public AttributeInfo {
	LocalVariableTableEntry* entries;
	uint16_t size;
};

struct LocalVariableTypeTableEntry {
	uint16_t startPc;
	uint16_t length;
	uint16_t nameIndex;
	uint16_t signatureIndex;
	uint16_t index;
};

struct LocalVariableTypeTableAttribute : public AttributeInfo {
	uint16_t size;
	LocalVariableTypeTableEntry* entries;
};

struct VerificationTypeInfo {
	uint8_t tag;
	uint16_t data;
};

enum StackMapFrameType : uint8_t {
	SameFrameType,
	SameLocals1StackItemFrameType,
	SameLocals1StackItemFrameExtendedType,
	ChopFrameType,
	SameFrameExtendedType,
	AppendFrameType,
	FullFrameType
};


struct StackMapFrame {
	StackMapFrameType frameType;
};

struct SameFrame : public StackMapFrame {
	uint8_t offsetDelta;
};

struct SameLocals1StackItemFrame : public StackMapFrame {
	uint16_t offsetDelta;
	VerificationTypeInfo stack;
};

struct SameLocals1StackItemFrameExtended : public StackMapFrame {
	uint16_t offsetDelta;
	VerificationTypeInfo stack;
};

struct ChopFrame : public StackMapFrame {
	uint16_t offsetDelta;
};

struct SameFrameExtended : public StackMapFrame {
	uint16_t offsetDelta;
};

struct AppendFrame : public StackMapFrame {
	uint16_t offsetDelta;
	// The size of this array is: frame_type - 251
	uint8_t numberOfLocals;
	VerificationTypeInfo* locals;
};

struct FullFrame : public StackMapFrame {
	uint16_t offsetDelta;
	uint16_t numberOfLocals;
	VerificationTypeInfo* locals;
	uint16_t numberOfStackItems;
	VerificationTypeInfo* stack;
};

struct StackMapTableAttribute : public AttributeInfo {
	uint16_t entriesCount; // Number of stack_map_frame entries
	StackMapFrame** entries;
};

struct ExceptionsAttribute : public AttributeInfo {
	uint16_t exceptionsCount;
	uint16_t* exceptionsIndexTable;
};

struct InnerClass {
	uint16_t innerClassInfoIndex;
	uint16_t outerClassInfoIndex;
	uint16_t innerNameIndex;
	uint16_t innerClassAccessFlags;
};

struct InnerClassesAttribute : public AttributeInfo {
	uint16_t numberOfClasses;
	InnerClass* classes;
};

struct BootstrapMethod {
	uint16_t bootstrapMethodRef;
	uint16_t numberofBootstrapArguments;
	uint16_t *bootstrapArguments;
};

struct BootstrapMethodsAttribute : public AttributeInfo {
	uint16_t numberOfBootstrapMethods;
	BootstrapMethod* bootstrapMethods;
};

struct DeprecatedAttribute : public AttributeInfo {};

struct Annotation;
struct ElementValue;

struct ElementValue {
	uint8_t tag;
	union {
		uint16_t const_value_index;
		struct {
			uint16_t type_name_index;
			uint16_t const_name_index;
		} enum_const_value;
		uint16_t class_info_index;
		Annotation* annotation_value;
		struct {
			uint16_t num_values;
			ElementValue* values;
		} array_value;
	} value;
};

struct ElementValuePair {
	uint16_t elementNameIndex;
	ElementValue value;
};

struct Annotation {
	uint16_t typeIndex;
	uint16_t numElementValuePairs;
	ElementValuePair *elementValuePairs;
};


struct RuntimeVisibleAnnotationsAttribute : public AttributeInfo {
	uint16_t annotationsCount;
	Annotation* annotations;
};

struct RuntimeInvisibleAnnotationsAttribute : public AttributeInfo {
	uint16_t annotationsCount;
	Annotation* annotations;
};

struct SyntheticAttribute : public AttributeInfo {};

struct SignatureAttribute : public AttributeInfo {
	uint16_t signatureIndex;
};

struct AnnotationDefaultAttribute : public AttributeInfo {
	ElementValue defaultValue;
};

struct EnclosingMethodAttribute : public AttributeInfo {
	uint16_t classIndex;
	uint16_t methodIndex;
};

struct SourceDebugExtensionAttribute : public AttributeInfo {
	uint8_t* debugExtension;
};

struct Parameter {
	uint16_t nameIndex;
	uint16_t accessFlags;
};

struct MethodParametersAttribute : public AttributeInfo {
	uint8_t parametersCount;
	Parameter* parameters;
};

struct Annotations {
	uint16_t numAnnotations;
	Annotation* annotations;
};

struct RuntimeVisibleParameterAnnotationsAttribute : public AttributeInfo {
	uint8_t numParameters;
	Annotations* parameterAnotations;
};

struct RuntimeInvisibleParameterAnnotationsAttribute : public RuntimeVisibleParameterAnnotationsAttribute {};

struct TypePathPath {
	uint8_t typePathKind;
	uint8_t typeArgumentIndex;
};

struct TypePath {
	uint8_t pathLength;
	TypePathPath* path;
};

struct TypeParameterTarget {
	uint8_t typeParameterIndex;
};

struct SuperTypeTarget {
	uint16_t superTypeIndex;
};

struct TypeParameterBoundTarget {
	uint8_t typeParameterIndex;
	uint8_t boundIndex;
};

struct EmptyTarget {
};

struct FormalParameterTarget {
	uint8_t formalParameterIndex;
};

struct ThrowsTarget {
	uint16_t throwsTypeIndex;
};

struct LocalVarTargetEntry {
	uint16_t startPc;
	uint16_t length;
	uint16_t index;
};

struct LocalVarTarget {
	uint16_t tableLength;
	LocalVarTargetEntry* table;
};

struct CatchTarget {
	uint16_t exceptionTableIndex;
};

struct OffsetTarget {
	uint16_t offset;
};

struct TypeArgumentTarget {
	uint16_t offset;
	uint8_t typeArgumentIndex;
};

struct TypeAnnotation {
	uint8_t targetType;
	union {
		TypeParameterTarget target;
		SuperTypeTarget superTypeTarget;
		TypeParameterBoundTarget typeParameterBoundTarget;
		EmptyTarget emptyTarget;
		FormalParameterTarget formalParameterTarget;
		ThrowsTarget throwsTarget;
		LocalVarTarget localVarTarget;
		CatchTarget catchTarget;
		OffsetTarget offsetTarget;
		TypeArgumentTarget typeArgumentTarget;
	} targetInfo;
	TypePath targetPath;
	uint16_t typeIndex;
	uint16_t numElementValuePairs;
	ElementValuePair* elementValuePairs;
};

struct RuntimeVisibleTypeAnnotationsAttribute : public AttributeInfo {
	uint16_t numAnnotations;
	TypeAnnotation* annotations;
};

struct RuntimeInvisibleTypeAnnotationsAttribute : public RuntimeVisibleTypeAnnotationsAttribute {
};