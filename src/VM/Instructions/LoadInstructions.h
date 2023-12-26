#pragma once

#include "Core.h"

class VM;
class VMThread;
class JavaHeap;

void iload(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void aload_i(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void iload_i(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void iaload(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void caload(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
