#pragma once

#include "Core.h"

class VM;
class VMThread;
class JavaHeap;

void gotoInstruction(INSTRUCTION_ARGS);
void ireturnInstruction(INSTRUCTION_ARGS);
void areturnInstruction(INSTRUCTION_ARGS);
void returnInstruction(INSTRUCTION_ARGS);
