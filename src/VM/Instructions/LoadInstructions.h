#pragma once

#include "Core.h"

class VM;
class VMThread;
class JavaHeap;

void iload(INSTRUCTION_ARGS);
void aload_i(INSTRUCTION_ARGS);
void iload_i(INSTRUCTION_ARGS);
void iaload(INSTRUCTION_ARGS);
void caload(INSTRUCTION_ARGS);
