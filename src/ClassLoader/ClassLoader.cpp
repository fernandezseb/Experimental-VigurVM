/*
 * Copyright (c) 2023-2024 Sebastiaan Fernandez.
 *
 * This file is part of VigurVM.
 *
 * VigurVM is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * VigurVM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with Foobar.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "ClassLoader.h"

#include <ctime>

#include "Core.h"
#include "DescriptorParser.h"
#include "Util.h"
#include "Platform.h"
#include "Memory.h"
#include "physfs.h"

void ClassLoader::checkMagicNumber(ByteArray& byteArray) {
    uint32_t magic = byteArray.readUnsignedInt();
    if (magic != MAGIC_NUMBER) {
        fprintf(stderr, "Error: Magic Number not OK.Exiting.\n");
        Platform::exitProgram(1);
    }
}

ConstantPool* ClassLoader::readConstantPool(ByteArray& byteArray)
{
    ConstantPool* constantPool = (ConstantPool*)m_memory->alloc(sizeof(ConstantPool));

    uint16_t cpCount = byteArray.readUnsignedShort();

    uint16_t arrCount = cpCount - 1;

    constantPool->constants = (ConstantPoolItem**) m_memory->alloc(sizeof(ConstantPoolItem*) * arrCount);
    constantPool->size = arrCount;

    for (uint32_t currentConstantIndex = 0; currentConstantIndex < arrCount; currentConstantIndex++) {
        uint8_t tag = byteArray.readUnsignedByte();

        ConstantPoolItem* constantPoolItem = readConstantPoolItem(tag, byteArray);

        constantPool->constants[currentConstantIndex] = constantPoolItem;

        // if tag is long or double we need to increment by 2
        if (tag == CT_LONG || tag == CT_DOUBLE) {
            currentConstantIndex++;
            constantPool->constants[currentConstantIndex] = 0;
        }

    }

    return constantPool;
}

ConstantPoolItem* ClassLoader::readConstantPoolItem(uint8_t tag, ByteArray& byteArray)
{
    ConstantPoolItem* item = nullptr;

    switch (tag) {
    case CT_METHODREF:
    {
        uint16_t classIndex = byteArray.readUnsignedShort();
        uint16_t nameAndTypeIndex = byteArray.readUnsignedShort();
        CPMethodRef* methodRef = (CPMethodRef*)m_memory->alloc(sizeof(CPMethodRef));
        methodRef->tag = tag;
        methodRef->classIndex = classIndex;
        methodRef->nameAndTypeIndex = nameAndTypeIndex;
        item = methodRef;
        break;
    }
    case CT_CLASS:
    {
        uint16_t nameIndex = byteArray.readUnsignedShort();
        
        CPClassInfo* classInfo = (CPClassInfo*)m_memory->alloc(sizeof(CPClassInfo));
        classInfo->tag = tag;
        classInfo->nameIndex = nameIndex;
        
        item = classInfo;
        break;
    }
    case CT_UTF8:
    {
        uint16_t size = byteArray.readUnsignedShort();
        uint16_t strBytes = size * sizeof(uint8_t) + 1u;
        uint8_t* buffer = (uint8_t*)m_memory->alloc(strBytes);
        byteArray.readBytes(buffer, size);
        buffer[strBytes - 1] = '\0';
        CPUTF8Info* itemUtf8 = (CPUTF8Info*)m_memory->alloc(sizeof(CPUTF8Info));
        itemUtf8->tag = tag;
        itemUtf8->length = strBytes;
        itemUtf8->bytes = buffer;
        item = itemUtf8;
        break;
    }
    case CT_NAMEANDTYPE:
    {
        uint16_t nameIndex = byteArray.readUnsignedShort();
        uint16_t descriptorIndex = byteArray.readUnsignedShort();
        CPNameAndTypeInfo* nameAndtype = (CPNameAndTypeInfo*)m_memory->alloc(sizeof(CPNameAndTypeInfo));
        nameAndtype->tag = tag;
        nameAndtype->nameIndex = nameIndex;
        nameAndtype->descriptorIndex = descriptorIndex;
        item = nameAndtype;
        break;
    }
    case CT_STRING:
    {
        uint16_t stringIndex = byteArray.readUnsignedShort();
        CPStringInfo* stringInfo = (CPStringInfo*)m_memory->alloc(sizeof(CPStringInfo));
        stringInfo->tag = tag;
        stringInfo->stringIndex = stringIndex;
        item = stringInfo;
        break;
    }
    case CT_FIELDREF:
    {
        // TODO: De-duplicate from methodref
        uint16_t classIndex = byteArray.readUnsignedShort();
        uint16_t nameAndTypeIndex = byteArray.readUnsignedShort();
        CPFieldRef* fieldRef = (CPFieldRef*)m_memory->alloc(sizeof(CPFieldRef));
        fieldRef->tag = tag;
        fieldRef->classIndex = classIndex;
        fieldRef->nameAndTypeIndex = nameAndTypeIndex;
        item = fieldRef;
        break;
    }
    case CT_INTERFACEMETHOD:
    {
        // TODO: De-duplicate from methodref
        uint16_t classIndex = byteArray.readUnsignedShort();
        uint16_t nameAndTypeIndex = byteArray.readUnsignedShort();
        CPInterfaceRef* interfaceRef = (CPInterfaceRef*)m_memory->alloc(sizeof(CPInterfaceRef));
        interfaceRef->tag = tag;
        interfaceRef->classIndex = classIndex;
        interfaceRef->nameAndTypeIndex = nameAndTypeIndex;
        item = interfaceRef;
        break;
    }
    case CT_INTEGER:
    {
        // TODO: Parse the int as the correct type
        uint32_t intBytes = byteArray.readUnsignedInt();
        CPIntegerInfo* integerInfo = (CPIntegerInfo*)m_memory->alloc(sizeof(CPIntegerInfo));
        integerInfo->tag = tag;
        integerInfo->bytes = intBytes;
        item = integerInfo;
        break;
    }
    case CT_FLOAT:
    {
        // TODO: Parse the int as the correct type
        uint32_t floatBytes = byteArray.readUnsignedInt();
        CPFloatInfo* floatInfo = (CPFloatInfo*)m_memory->alloc(sizeof(CPFloatInfo));
        floatInfo->tag = tag;
        floatInfo->bytes = floatBytes;
        item = floatInfo;
        break;
    }
    case CT_LONG:
    {
        uint32_t highBytes = byteArray.readUnsignedInt();
        uint32_t lowBytes = byteArray.readUnsignedInt();
        CPLongInfo* longInfo = (CPLongInfo*)m_memory->alloc(sizeof(CPLongInfo));
        longInfo->tag = tag;
        longInfo->highBytes = highBytes;
        longInfo->lowBytes = lowBytes;
        item = longInfo;
        break;
    }
    case CT_DOUBLE:
    {
        uint32_t highBytes = byteArray.readUnsignedInt();
        uint32_t lowBytes = byteArray.readUnsignedInt();
        CPDoubleInfo* doubleInfo = (CPDoubleInfo*)m_memory->alloc(sizeof(CPDoubleInfo));
        doubleInfo->tag = tag;
        doubleInfo->highBytes = highBytes;
        doubleInfo->lowBytes = lowBytes;
        item = doubleInfo;
        break;
    }
    case CT_INVOKEDYNAMIC:
    {
        CPInvokeDynamicInfo* idInfo = (CPInvokeDynamicInfo*)m_memory->alloc(sizeof(CPInvokeDynamicInfo));
        idInfo->tag = tag;
        idInfo->bootstrapMethodAttrIndex = byteArray.readUnsignedShort();
        idInfo->nameAndTypeIndex = byteArray.readUnsignedShort();
        item = idInfo;
        break;
    }
    case CT_METHODHANDLE:
    {
        CPMethodHandleInfo* handleInfo = (CPMethodHandleInfo*)m_memory->alloc(sizeof(CPMethodHandleInfo));
        handleInfo->tag = tag;
        handleInfo->referenceKind = byteArray.readUnsignedByte();
        handleInfo->referenceIndex = byteArray.readUnsignedShort();
        item = handleInfo;
        break;
    }
    case CT_METHODTYPE:
    {
        CPMethodTypeInfo* handleInfo = (CPMethodTypeInfo*)m_memory->alloc(sizeof(CPMethodTypeInfo));
        handleInfo->tag = tag;
        handleInfo->descriptorIndex = byteArray.readUnsignedShort();
        item = handleInfo;
        break;
    }
    default:
    {
        fprintf(stderr, "Error: Unidentified constant pool item detected with tag: %" PRIu8 "\n", tag);
        Platform::exitProgram(5);
    }
    }

    return item;
}

ClassInfo* ClassLoader::readClass(ByteArray& byteArray)
{
    checkMagicNumber(byteArray);

    ClassInfo* classInfo = (ClassInfo*) m_memory->alloc(sizeof(ClassInfo));

    classInfo->minorVersion = byteArray.readUnsignedShort();
    classInfo->majorVersion = byteArray.readUnsignedShort();
    classInfo->constantPool = readConstantPool(byteArray);
    classInfo->accessFlags = byteArray.readUnsignedShort();
    classInfo->thisClass = byteArray.readUnsignedShort();
    classInfo->superClass = byteArray.readUnsignedShort();
    
    uint16_t interfacesCount = byteArray.readUnsignedShort();
    classInfo->interfaces = readInterfaces(byteArray, interfacesCount);
    classInfo->interfacesCount = interfacesCount;

    uint16_t fieldsCount = byteArray.readUnsignedShort();
    classInfo->fields = readFields(byteArray, classInfo->constantPool, fieldsCount);
    classInfo->fieldsCount = fieldsCount;

    uint16_t methodsCount = byteArray.readUnsignedShort();
    classInfo->methods = readMethods(byteArray, classInfo->constantPool, methodsCount);
    classInfo->methodCount = methodsCount;

    AttributeCollection* attributeInfo = AttributeParser::readAttributes(byteArray, classInfo->constantPool, m_memory);
    classInfo->attributes = attributeInfo;
    AttributeSourceFile* sourceFile = (AttributeSourceFile*) attributeInfo->findAttributeWithName(classInfo->constantPool, "SourceFile");

    if (sourceFile != NULL) {
        const char* sourceFileString = classInfo->constantPool->getString(sourceFile->sourceFileIndex);
        classInfo->sourceFile = (char*)sourceFileString;
    }

    return classInfo;
}

ClassInfo* ClassLoader::readClass(const char* className, Memory* memory, const char* classPath)
{
    this->m_memory = memory;
    char name[300] = {0};
    snprintf(name, 300, "%s.class", className);

    PHYSFS_addToSearchPath(".", 1);
    PHYSFS_addToSearchPath("extern/classpath/rt.jar/rt.jar", 1);
    const int exists = PHYSFS_exists(name);
    if (!exists)
    {
        fprintf(stderr, "Class file not found for className: %s\n", className);
        Platform::exitProgram(6);
    }

    PHYSFS_file* file = PHYSFS_openRead(name);
    const u8 size = PHYSFS_fileLength(file);
    const auto fileContent = static_cast<u1*>(Platform::allocateMemory(size, 0));

    PHYSFS_read(file, fileContent, 1, castToU4(size));

    ByteArray byteArray(fileContent, size);

    ClassInfo* classInfo = readClass(byteArray);
    classInfo->memory = this->m_memory;
    const char* path = PHYSFS_getRealDir(name);
    classInfo->filePath = const_cast<char*>(path);
    classInfo->size = byteArray.getSize();
    const i8 modTime = PHYSFS_getLastModTime(name);
    char time[50];
    strftime(time, 20, "%d-%m-%y", localtime(&modTime));
    strncpy(classInfo->lastModifiedString, time, 50);
    PHYSFS_close(file);
    Platform::freeMemory(fileContent);

    return classInfo;
}

uint16_t* ClassLoader::readInterfaces(ByteArray& byteArray, uint16_t interfacesCount)
{
    uint16_t* interfaces = (uint16_t*) m_memory->alloc(sizeof(uint16_t) * interfacesCount);

    for (uint16_t currentInterface = 0; currentInterface < interfacesCount; currentInterface++) {
        uint16_t interfaceIndex = byteArray.readUnsignedShort();
        interfaces[currentInterface] = interfaceIndex;
    }

    return interfaces;
}

FieldInfo** ClassLoader::readFields(ByteArray& byteArray, ConstantPool* constantPool, uint16_t fieldsCount)
{
    FieldInfo** fields = (FieldInfo**)m_memory->alloc(sizeof(FieldInfo*) * fieldsCount);

    for (uint16_t currentField = 0; currentField < fieldsCount; ++currentField) {
        uint16_t accessFlags = byteArray.readUnsignedShort();
        uint16_t nameIndex = byteArray.readUnsignedShort();
        uint16_t descriptorIndex = byteArray.readUnsignedShort();
        AttributeCollection* attributeInfo = AttributeParser::readAttributes(byteArray, constantPool, m_memory);
        FieldInfo* fieldInfo = (FieldInfo*) m_memory->alloc(sizeof(FieldInfo));
        fieldInfo->accessFlags = accessFlags;
        fieldInfo->descriptorIndex = descriptorIndex;
        fieldInfo->nameIndex = nameIndex;
        fieldInfo->isPrivate = ((accessFlags & ACC_PRIVATE) == ACC_PRIVATE);
        fieldInfo->attributes = attributeInfo;
        fields[currentField] = fieldInfo;
    }

    return fields;
}

void ClassLoader::parseDescriptor(const char* descriptor, MethodInfo* method)
{
    Descriptor desc = DescriptorParser::parseDescriptor((char*)descriptor, m_memory);
    method->returnType = desc.returnType;

    method->args = desc.args;
    method->argsCount = desc.argsCount;
}

MethodInfo** ClassLoader::readMethods(ByteArray& byteArray, ConstantPool* constantPool, uint16_t methodCount)
{
    MethodInfo** methods = (MethodInfo**)m_memory->alloc(sizeof(MethodInfo*) * methodCount);

    for (uint16_t currentMethod = 0; currentMethod < methodCount; ++currentMethod) {
        uint16_t accessFlags = byteArray.readUnsignedShort();
        uint16_t nameIndex = byteArray.readUnsignedShort();
        uint16_t descriptorIndex = byteArray.readUnsignedShort();
        AttributeCollection* attributes = AttributeParser::readAttributes(byteArray, constantPool, m_memory);
        MethodInfo* info = (MethodInfo*) m_memory->alloc(sizeof(MethodInfo));
        info->accessFlags = accessFlags;
        info->nameIndex = nameIndex;
        info->descriptorIndex = descriptorIndex;
        info->code = 0;
        info->attributes = attributes;
        bool isNative = ((accessFlags & ACC_NATIVE) == ACC_NATIVE);
        bool isAbstract = ((accessFlags & ACC_ABSTRACT) == ACC_ABSTRACT);
        if (!(isNative || isAbstract)) {
            info->code = (AttributeCode*) attributes->findAttributeWithName(constantPool, "Code");
        }
        parseDescriptor(constantPool->getString(descriptorIndex), info);

        const char* name = constantPool->getString(nameIndex);
        info->name = name;

        methods[currentMethod] = info;
    }

    return methods;
}
