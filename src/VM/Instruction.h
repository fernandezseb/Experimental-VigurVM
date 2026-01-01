#pragma once

#include "Core.h"
#include "Data/Opcode.h"

#include <string>

class VM;
class JavaHeap;
class VMThread;

struct InstructionInput
{
    u1 *args;
    u2 argsCount;
    i1 arg;
    VMThread* thread;
};

struct Instruction
{
    Opcode opcode;
    u1 args;
    std::string_view name;
    i1 arg;
    void(*instructionFunction)(const InstructionInput& input);
};