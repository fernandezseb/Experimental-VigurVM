#pragma once

#include "Core.h"

class VM;
class VMThread;
class JavaHeap;

void istore(INSTRUCTION_ARGS);
void astore(INSTRUCTION_ARGS);
void istore_i(INSTRUCTION_ARGS);
void astore_i(INSTRUCTION_ARGS);
void iastore(INSTRUCTION_ARGS);
void aastore(INSTRUCTION_ARGS);
void castore(INSTRUCTION_ARGS);
