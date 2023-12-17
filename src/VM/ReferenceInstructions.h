#pragma once

#include "Core.h"

class VM;
class VMThread;
class JavaHeap;

void getstatic(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void putstatic(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void putfield(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);