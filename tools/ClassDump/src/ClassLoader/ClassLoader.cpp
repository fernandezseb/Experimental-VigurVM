#include "ClassLoader.h"

#include "Core.h"
#include "DescriptorParser.h"
#include "md5/md5.h"
#include "Util.h"
#include "Platform.h"
#include "Memory.h"

void ClassLoader::checkMagicNumber(ByteArray& byteArray) {
    uint32_t magic = byteArray.readUnsignedInt();
    if (magic != MAGIC_NUMBER) {
        fprintf(stderr, "Error: Magic Number not OK.Exiting.\n");
        Platform::exitProgram(1);
    }
}

ConstantPool* ClassLoader::readConstantPool(ByteArray& byteArray)
{
    ConstantPool* constantPool = (ConstantPool*)memory->alloc(sizeof(ConstantPool));

    uint16_t cpCount = byteArray.readUnsignedShort();

    uint16_t arrCount = cpCount - 1;

    constantPool->constants = (ConstantPoolItem**) memory->alloc(sizeof(ConstantPoolItem*) * arrCount);
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
        CPMethodRef* methodRef = (CPMethodRef*)memory->alloc(sizeof(CPMethodRef));
        methodRef->tag = tag;
        methodRef->classIndex = classIndex;
        methodRef->nameAndTypeIndex = nameAndTypeIndex;
        item = methodRef;
        break;
    }
    case CT_CLASS:
    {
        uint16_t nameIndex = byteArray.readUnsignedShort();
        
        CPClassInfo* classInfo = (CPClassInfo*)memory->alloc(sizeof(CPClassInfo));
        classInfo->tag = tag;
        classInfo->nameIndex = nameIndex;
        
        item = classInfo;
        break;
    }
    case CT_UTF8:
    {
        uint16_t size = byteArray.readUnsignedShort();
        uint16_t strBytes = size * sizeof(uint8_t) + 1u;
        uint8_t* buffer = (uint8_t*)memory->alloc(strBytes);
        byteArray.readBytes(buffer, size);
        buffer[strBytes - 1] = '\0';
        CPUTF8Info* itemUtf8 = (CPUTF8Info*)memory->alloc(sizeof(CPUTF8Info));
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
        CPNameAndTypeInfo* nameAndtype = (CPNameAndTypeInfo*)memory->alloc(sizeof(CPNameAndTypeInfo));
        nameAndtype->tag = tag;
        nameAndtype->nameIndex = nameIndex;
        nameAndtype->descriptorIndex = descriptorIndex;
        item = nameAndtype;
        break;
    }
    case CT_STRING:
    {
        uint16_t stringIndex = byteArray.readUnsignedShort();
        CPStringInfo* stringInfo = (CPStringInfo*)memory->alloc(sizeof(CPStringInfo));
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
        CPFieldRef* fieldRef = (CPFieldRef*)memory->alloc(sizeof(CPFieldRef));
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
        CPInterfaceRef* interfaceRef = (CPInterfaceRef*)memory->alloc(sizeof(CPInterfaceRef));
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
        CPIntegerInfo* integerInfo = (CPIntegerInfo*)memory->alloc(sizeof(CPIntegerInfo));
        integerInfo->tag = tag;
        integerInfo->bytes = intBytes;
        item = integerInfo;
        break;
    }
    case CT_FLOAT:
    {
        // TODO: Parse the int as the correct type
        uint32_t floatBytes = byteArray.readUnsignedInt();
        CPFloatInfo* floatInfo = (CPFloatInfo*)memory->alloc(sizeof(CPFloatInfo));
        floatInfo->tag = tag;
        floatInfo->bytes = floatBytes;
        item = floatInfo;
        break;
    }
    case CT_LONG:
    {
        uint32_t highBytes = byteArray.readUnsignedInt();
        uint32_t lowBytes = byteArray.readUnsignedInt();
        CPLongInfo* longInfo = (CPLongInfo*)memory->alloc(sizeof(CPLongInfo));
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
        CPDoubleInfo* doubleInfo = (CPDoubleInfo*)memory->alloc(sizeof(CPDoubleInfo));
        doubleInfo->tag = tag;
        doubleInfo->highBytes = highBytes;
        doubleInfo->lowBytes = lowBytes;
        item = doubleInfo;
        break;
    }
    case CT_INVOKEDYNAMIC:
    {
        CPInvokeDynamicInfo* idInfo = (CPInvokeDynamicInfo*)memory->alloc(sizeof(CPInvokeDynamicInfo));
        idInfo->tag = tag;
        idInfo->bootstrapMethodAttrIndex = byteArray.readUnsignedShort();
        idInfo->nameAndTypeIndex = byteArray.readUnsignedShort();
        item = idInfo;
        break;
    }
    case CT_METHODHANDLE:
    {
        CPMethodHandleInfo* handleInfo = (CPMethodHandleInfo*)memory->alloc(sizeof(CPMethodHandleInfo));
        handleInfo->tag = tag;
        handleInfo->referenceKind = byteArray.readUnsignedByte();
        handleInfo->referenceIndex = byteArray.readUnsignedShort();
        item = handleInfo;
        break;
    }
    case CT_METHODTYPE:
    {
        CPMethodTypeInfo* handleInfo = (CPMethodTypeInfo*)memory->alloc(sizeof(CPMethodTypeInfo));
        handleInfo->tag = tag;
        handleInfo->descriptorIndex = byteArray.readUnsignedShort();
        item = handleInfo;
        break;
    }
    default:
    {
        fprintf(stderr, "Error: Unidentified constant pool item detected with tag: %" PRIu8 "\n", tag);
        Platform::exitProgram(-5);
    }
    }

    return item;
}

ClassInfo* ClassLoader::readClass(ByteArray& byteArray)
{
    checkMagicNumber(byteArray);

    ClassInfo* classInfo = (ClassInfo*) memory->alloc(sizeof(ClassInfo));

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

    AttributeCollection* attributeInfo = AttributeParser::readAttributes(byteArray, classInfo->constantPool, memory);
    classInfo->attributes = attributeInfo;
    AttributeSourceFile* sourceFile = (AttributeSourceFile*) attributeInfo->findAttributeWithName(classInfo->constantPool, "SourceFile");

    if (sourceFile != NULL) {
        const char* sourceFileString = classInfo->constantPool->getString(sourceFile->sourceFileIndex);
        classInfo->sourceFile = (char*)sourceFileString;
    }

    return classInfo;
}

ClassInfo* ClassLoader::readClass(const char* className, Memory* memory)
{
    this->memory = memory;
    PlatformFile *file = Platform::getFile(className);

    size_t size;
    uint8_t* fileContent = Platform::readEntireFile(file, &size);
    ByteArray byteArray(fileContent, size);

    ClassInfo* classInfo = readClass(byteArray);
    classInfo->memory = this->memory;
    char* path = (char*)this->memory->alloc(500);
    Platform::getFullPath(file, path);
    // TODO: Resize path string memory
    classInfo->filePath = path;
    classInfo->size = byteArray.getSize();
    Platform::getLastModifiedString(file, classInfo->lastModifiedString);
    
    char md5Buffer[33];
    md5(byteArray.bytes, byteArray.getSize(), md5Buffer);
    strcpy(classInfo->md5, md5Buffer);

    Platform::closeFile(file);
    Platform::freeMemory(fileContent);

    return classInfo;
}

uint16_t* ClassLoader::readInterfaces(ByteArray& byteArray, uint16_t interfacesCount)
{
    uint16_t* interfaces = (uint16_t*) memory->alloc(sizeof(uint16_t) * interfacesCount);

    for (uint16_t currentInterface = 0; currentInterface < interfacesCount; currentInterface++) {
        uint16_t interfaceIndex = byteArray.readUnsignedShort();
        interfaces[currentInterface] = interfaceIndex;
    }

    return interfaces;
}

FieldInfo** ClassLoader::readFields(ByteArray& byteArray, ConstantPool* constantPool, uint16_t fieldsCount)
{
    FieldInfo** fields = (FieldInfo**)memory->alloc(sizeof(FieldInfo*) * fieldsCount);

    for (uint16_t currentField = 0; currentField < fieldsCount; ++currentField) {
        uint16_t accessFlags = byteArray.readUnsignedShort();
        uint16_t nameIndex = byteArray.readUnsignedShort();
        uint16_t descriptorIndex = byteArray.readUnsignedShort();
        AttributeCollection* attributeInfo = AttributeParser::readAttributes(byteArray, constantPool, memory);
        FieldInfo* fieldInfo = (FieldInfo*) memory->alloc(sizeof(FieldInfo));
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
    Descriptor desc = DescriptorParser::parseDescriptor((char*)descriptor, memory);
    method->returnType = desc.returnType;

    method->args = desc.args;
    method->argsCount = desc.argsCount;
}

MethodInfo** ClassLoader::readMethods(ByteArray& byteArray, ConstantPool* constantPool, uint16_t methodCount)
{
    MethodInfo** methods = (MethodInfo**)memory->alloc(sizeof(MethodInfo*) * methodCount);

    for (uint16_t currentMethod = 0; currentMethod < methodCount; ++currentMethod) {
        uint16_t accessFlags = byteArray.readUnsignedShort();
        uint16_t nameIndex = byteArray.readUnsignedShort();
        uint16_t descriptorIndex = byteArray.readUnsignedShort();
        AttributeCollection* attributes = AttributeParser::readAttributes(byteArray, constantPool, memory);
        MethodInfo* info = (MethodInfo*) memory->alloc(sizeof(MethodInfo));
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
        info->name = constantPool->getString(nameIndex);

        methods[currentMethod] = info;
    }

    return methods;
}
