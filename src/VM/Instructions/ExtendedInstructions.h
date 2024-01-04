#pragma once

#include "Core.h"

class VM;
class VMThread;
class JavaHeap;

void ifnull(INSTRUCTION_ARGS);
void ifnonnull(INSTRUCTION_ARGS);