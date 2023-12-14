#pragma once

#include "Core.h"

class VMThread;
class JavaHeap;

void aconst_null(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread);
void iconst_i(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread);
void lconst_i(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread);
void fconst_i(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread);
// void dconst_i(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread);
