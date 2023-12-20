#pragma once

#include "Core.h"

class VM;
class VMThread;
class JavaHeap;

void pop(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
void dup(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);