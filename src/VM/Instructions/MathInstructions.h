#pragma once

#include "Core.h"

class VM;
class VMThread;
class JavaHeap;

void iadd(INSTRUCTION_ARGS);
void ladd(INSTRUCTION_ARGS);
void isub(INSTRUCTION_ARGS);
void imul(INSTRUCTION_ARGS);
void fmul(INSTRUCTION_ARGS);
void irem(INSTRUCTION_ARGS);
void lshl(INSTRUCTION_ARGS);
void ishl(INSTRUCTION_ARGS);
void iushr(INSTRUCTION_ARGS);
void iand(INSTRUCTION_ARGS);
void land(INSTRUCTION_ARGS);
void ior(INSTRUCTION_ARGS);
void ixor(INSTRUCTION_ARGS);
void iinc(INSTRUCTION_ARGS);
