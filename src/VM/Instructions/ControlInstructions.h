#pragma once

#include "Core.h"

class VM;
class VMThread;
class JavaHeap;

void gotoInstruction(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void ireturnInstruction(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void areturnInstruction(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void returnInstruction(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
