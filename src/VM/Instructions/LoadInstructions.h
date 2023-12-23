#pragma once

#include "Core.h"

class VM;
class VMThread;
class JavaHeap;

void iload(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void aload_i(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void iload_i(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void iaload(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void caload(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);