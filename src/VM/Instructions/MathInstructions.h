#pragma once

#include "../Instruction.h"

void iadd(const InstructionInput& input);
void ladd(const InstructionInput& input);
void isub(const InstructionInput& input);
void imul(const InstructionInput& input);
void fmul(const InstructionInput& input);
void irem(const InstructionInput& input);
void lshl(const InstructionInput& input);
void ishl(const InstructionInput& input);
void ishr(const InstructionInput& input);
void iushr(const InstructionInput& input);
void iand(const InstructionInput& input);
void land(const InstructionInput& input);
void ior(const InstructionInput& input);
void ixor(const InstructionInput& input);
void iinc(const InstructionInput& input);
