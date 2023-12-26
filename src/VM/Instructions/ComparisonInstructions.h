#pragma once

#include "Core.h"

class VM;
class VMThread;
class JavaHeap;

void ifeq(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void ifne(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void ifge(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void ifgt(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void ifle(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void if_icmpne(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void if_icmplt(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void if_icmpgt(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void if_icmple(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void if_acmpne(const u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
