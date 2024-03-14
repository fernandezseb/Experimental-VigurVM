#pragma once

#include "../Instruction.h"

void getstatic(const InstructionInput& input);
void putstatic(const InstructionInput& input);
void getfield(const InstructionInput& input);
void putfield(const InstructionInput& input);
void invokevirtual(const InstructionInput& input);
void invokespecial(const InstructionInput& input);
void invokestatic(const InstructionInput& input);
void invokeinterface(const InstructionInput& input);
void newInstruction(const InstructionInput& input);
void newarray(const InstructionInput& input);
void anewarray(const InstructionInput& input);
void arraylength(const InstructionInput& input);
void checkCast(const InstructionInput& input);
void instanceof(const InstructionInput& input);
void monitorenter(const InstructionInput& input);
void monitorexit(const InstructionInput& input);