#pragma once

#include "Core.h"

class VM;
class VMThread;
class JavaHeap;

void getstatic(INSTRUCTION_ARGS);
void putstatic(INSTRUCTION_ARGS);
void getfield(INSTRUCTION_ARGS);
void putfield(INSTRUCTION_ARGS);
void invokevirtual(INSTRUCTION_ARGS);
void invokespecial(INSTRUCTION_ARGS);
void invokestatic(INSTRUCTION_ARGS);
void invokeinterface(INSTRUCTION_ARGS);
void newInstruction(INSTRUCTION_ARGS);
void newarray(INSTRUCTION_ARGS);
void anewarray(INSTRUCTION_ARGS);
void arraylength(INSTRUCTION_ARGS);
void checkCast(INSTRUCTION_ARGS);
void instanceof(INSTRUCTION_ARGS);
void monitorenter(INSTRUCTION_ARGS);
void monitorexit(INSTRUCTION_ARGS);