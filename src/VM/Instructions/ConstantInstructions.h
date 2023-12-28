#pragma once

#include "Core.h"

class VMThread;
class JavaHeap;
class VM;

void nop(INSTRUCTION_ARGS);
void aconst_null(INSTRUCTION_ARGS);
void iconst_i(INSTRUCTION_ARGS);
void lconst_i(INSTRUCTION_ARGS);
void fconst_i(INSTRUCTION_ARGS);
void dconst_i(INSTRUCTION_ARGS);
void bipush(INSTRUCTION_ARGS);
void sipush(INSTRUCTION_ARGS);
void ldc(INSTRUCTION_ARGS);
void ldc_w(INSTRUCTION_ARGS);
void ldc2_w(INSTRUCTION_ARGS);