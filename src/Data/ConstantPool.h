#pragma once

#include "Core.h"
#include "Platform.h"

enum ConstantType : uint8_t {
	CT_UTF8            = 1u,
	CT_INTEGER         = 3u,
	CT_FLOAT           = 4u,
	CT_LONG            = 5u,
	CT_DOUBLE          = 6u,
	CT_CLASS           = 7u,
	CT_STRING          = 8u,
	CT_FIELDREF        = 9u,
	CT_METHODREF       = 10u,
	CT_INTERFACEMETHOD = 11u,
	CT_NAMEANDTYPE     = 12u,
	CT_METHODHANDLE	   = 15u,
	CT_METHODTYPE      = 16u,
	CT_INVOKEDYNAMIC   = 18u
};

enum AccessFlag : uint16_t {
	ACC_PUBLIC =       0x0001,
	ACC_PRIVATE =      0x0002,
	ACC_PROTECTED =    0x0004,
	ACC_STATIC =       0x0008,
	ACC_FINAL =        0x0010,
	ACC_SYNCHRONIZED = 0x0020,
	ACC_BRIDGE =       0x0040,
	ACC_VOLATILE =     0x0040,
	ACC_VARARGS =      0x0080,
	ACC_TRANSIENT =    0x0080,
	ACC_SUPER =        0x0020,
	ACC_NATIVE =       0x0100,
	ACC_INTERFACE =    0x0200,
	ACC_ABSTRACT =     0x0400,
	ACC_STRICT =       0x0800,
	ACC_SYNTHETIC =    0x1000,
	ACC_ANNOTATION =   0x2000,
	ACC_ENUM =         0x4000
};

class ConstantPool;

struct ConstantPoolItem {
	uint8_t tag;

	ConstantType getType() const {
		return (ConstantType)tag;
	}
};

struct ConstantPoolNamedReference : public ConstantPoolItem {
	uint16_t classIndex;
	uint16_t nameAndTypeIndex;
};

struct CPMethodRef : public ConstantPoolNamedReference {
};

struct CPFieldRef : public ConstantPoolNamedReference {
};

struct CPInterfaceRef : public ConstantPoolNamedReference {
};

struct CPClassInfo : public ConstantPoolItem {
	uint16_t nameIndex;
};

struct CPIntegerInfo : public ConstantPoolItem {
	uint32_t bytes;
};

struct CPFloatInfo : public ConstantPoolItem {
	uint32_t bytes;
};

struct CPLongInfo : public ConstantPoolItem {
	uint32_t highBytes;
	uint32_t lowBytes;
};

struct CPDoubleInfo : public ConstantPoolItem {
	uint32_t highBytes;
	uint32_t lowBytes;
};

struct CPUTF8Info : public ConstantPoolItem {
	uint16_t length;
	uint8_t* bytes;
};

struct CPNameAndTypeInfo : public ConstantPoolItem {
	uint16_t nameIndex;
	uint16_t descriptorIndex;
};

struct CPStringInfo : public ConstantPoolItem {
	uint16_t stringIndex;
};

struct CPInvokeDynamicInfo : public ConstantPoolItem {
	uint16_t bootstrapMethodAttrIndex;
	uint16_t nameAndTypeIndex;
};

struct CPMethodHandleInfo : public ConstantPoolItem {
	uint8_t referenceKind;
	uint16_t referenceIndex;
};

struct CPMethodTypeInfo : public ConstantPoolItem {
	uint16_t descriptorIndex;
};

class ConstantPool {
private:
	void checkIndex(uint16_t index) const;
public:
	ConstantPoolItem** constants;
	uint16_t size;
public:
	char* getString(uint16_t index) const;
	CPClassInfo* getClassInfo(uint16_t index) const;
};