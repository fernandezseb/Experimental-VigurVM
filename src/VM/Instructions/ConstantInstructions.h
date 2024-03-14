#pragma once

#include "../Instruction.h"

void nop(const InstructionInput& input);
void aconst_null(const InstructionInput& input);
void iconst_i(const InstructionInput& input);
void lconst_i(const InstructionInput& input);
void fconst_i(const InstructionInput& input);
void dconst_i(const InstructionInput& input);
void bipush(const InstructionInput& input);
void sipush(const InstructionInput& input);
void ldc(const InstructionInput& input);
void ldc_w(const InstructionInput& input);
void ldc2_w(const InstructionInput& input);