#include "Attribute.h"

AttributeInfo* AttributeCollection::findAttributeWithName(ConstantPool* constantPool, const char* name) const
{
	AttributeInfo* attrib = 0;

	for (int currentAttrib = 0; currentAttrib < attributesCount; ++currentAttrib) {
		attrib = attributes[currentAttrib];
		if (strcmp(constantPool->getString(attrib->attributeNameIndex), name) == 0) {
			return attrib;
		}
	}

	return attrib;
}