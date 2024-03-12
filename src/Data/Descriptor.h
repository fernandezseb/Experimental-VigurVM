#pragma once

#include "Core.h"

#include <string_view>

struct Descriptor {
	char** args;
	uint16_t argsCount;
	char* returnType;
};

static std::string_view getTypeAsString(const std::string_view descriptor)
{
	if (descriptor == "I")
	{
		return "int";
	}
	if (descriptor == "J")
	{
		return "float";
	}
	if (descriptor == "Z")
	{
		return "boolean";
	}
	if (descriptor == "B")
	{
		return "byte";
	}
	if (descriptor == "C")
	{
		return "char";
	}
	if (descriptor == "S")
	{
		return "short";
	}
	if (descriptor.starts_with("L") && descriptor.ends_with(";"))
	{
		std::string_view view = descriptor.substr(1, descriptor.size()-2);
		return view;
	}
	return descriptor;
}