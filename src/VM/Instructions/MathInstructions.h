#pragma once

#include "Core.h"

class VM;
class VMThread;
class JavaHeap;

void iadd(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void isub(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void imul(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void ishl(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void iushr(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void iinc(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
