#pragma once

#include "Core.h"

class VM;
class VMThread;
class JavaHeap;

void iadd(INSTRUCTION_ARGS);
void isub(INSTRUCTION_ARGS);
void imul(INSTRUCTION_ARGS);
void ishl(INSTRUCTION_ARGS);
void iushr(INSTRUCTION_ARGS);
void iinc(INSTRUCTION_ARGS);
