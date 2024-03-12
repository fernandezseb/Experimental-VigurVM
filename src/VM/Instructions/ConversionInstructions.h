#pragma once

#include "Core.h"

class VM;
class VMThread;
class JavaHeap;

void i2l(INSTRUCTION_ARGS);
void i2f(INSTRUCTION_ARGS);
void f2i(INSTRUCTION_ARGS);
void i2c(INSTRUCTION_ARGS);
