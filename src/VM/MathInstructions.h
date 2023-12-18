#pragma once

#include "Core.h"

class VM;
class VMThread;
class JavaHeap;

void iadd(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void isub(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void ishl(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void iinc(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
