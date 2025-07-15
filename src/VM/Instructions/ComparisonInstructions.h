#pragma once

#include "../Instruction.h"


void lcmp(const InstructionInput& input);
void fcmpl(const InstructionInput& input);
void fcmpg(const InstructionInput& input);
void ifeq(const InstructionInput& input);
void ifne(const InstructionInput& input);
void iflt(const InstructionInput& input);
void ifge(const InstructionInput& input);
void ifgt(const InstructionInput& input);
void ifle(const InstructionInput& input);
void if_icmpeq(const InstructionInput& input);
void if_icmpne(const InstructionInput& input);
void if_icmplt(const InstructionInput& input);
void if_icmpge(const InstructionInput& input);
void if_icmpgt(const InstructionInput& input);
void if_icmple(const InstructionInput& input);
void if_acmpeq(const InstructionInput& input);
void if_acmpne(const InstructionInput& input);
