#pragma once

#include "Variable.h"

#include "VM/VM.h"
#include "Error.h"
#include "Platform.h"

#include <utility>

u1 getCategoryFromVariableType(const VariableType type) noexcept
{
    if (type == VariableType_DOUBLE || type == VariableType_LONG)
    {
        return 2;
    }

    return 1;
}

u1 getDescriptorVarCategory(const std::string_view descriptor) noexcept
{
    {
        // Longs and doubles use two
        if (descriptor[0] == 'J' || descriptor[0] == 'D')
        {
            return 2;
        }
        return 1;
    }
}

void vdata::checkType(const VariableType expectedType) const
{
    if (this->type != expectedType)
    {
        VM::get()->getCurrentVMThread()->internalError("Error: Type mismatch", ErrorCode::TYPE_MISMATCH);
    }
}

VariableType fromDescriptor(const std::string_view descriptor) noexcept
{
    if (descriptor == "I")
    {
        return VariableType_INT;
    }
    if (descriptor ==  "Z")
    {
        return VariableType_INT;
    }
    if (descriptor == "B")
    {
        return VariableType_INT;
    }
    if (descriptor == "J")
    {
        return VariableType_LONG;
    }
    if (descriptor ==  "D")
    {
        return VariableType_DOUBLE;
    }
    if (descriptor[0] == '[')
    {
        return VariableType_REFERENCE;
    }
    if (descriptor[0] == 'L')
    {
        return VariableType_REFERENCE;
    }
    if (descriptor[0] == 'F') {
        return VariableType_FLOAT;
    }
    if (descriptor[0] == 'C')
    {
        return VariableType_INT;
    }
    fprintf(stderr, "Error: Couldn't construct data for descriptor type: %s\n", descriptor.data());
    Platform::exitProgram(7);
    std::unreachable();
}
