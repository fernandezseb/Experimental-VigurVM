#pragma once

#include "Core.h"

class VM;
class VMThread;
class JavaHeap;

void fcmpl(INSTRUCTION_ARGS);
void fcmpg(INSTRUCTION_ARGS);
void ifeq(INSTRUCTION_ARGS);
void ifne(INSTRUCTION_ARGS);
void iflt(INSTRUCTION_ARGS);
void ifge(INSTRUCTION_ARGS);
void ifgt(INSTRUCTION_ARGS);
void ifle(INSTRUCTION_ARGS);
void if_icmpeq(INSTRUCTION_ARGS);
void if_icmpne(INSTRUCTION_ARGS);
void if_icmplt(INSTRUCTION_ARGS);
void if_icmpge(INSTRUCTION_ARGS);
void if_icmpgt(INSTRUCTION_ARGS);
void if_icmple(INSTRUCTION_ARGS);
void if_acmpeq(INSTRUCTION_ARGS);
void if_acmpne(INSTRUCTION_ARGS);
