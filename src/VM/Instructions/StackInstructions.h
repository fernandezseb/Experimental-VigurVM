#pragma once

#include "Core.h"

class VM;
class VMThread;
class JavaHeap;

void pop(INSTRUCTION_ARGS);
void dup(INSTRUCTION_ARGS);