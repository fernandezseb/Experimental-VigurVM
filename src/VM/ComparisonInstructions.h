#pragma once

#include "Core.h"

class VM;
class VMThread;
class JavaHeap;

void ifge(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void ifle(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void if_icmpne(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void if_icmpgt(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void if_icmple(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
