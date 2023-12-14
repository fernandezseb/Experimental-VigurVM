#pragma once

#include "Core.h"

class VMThread;
class JavaHeap;

void nop(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread);
void aconst_null(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread);
void iconst_i(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread);
void lconst_i(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread);
void fconst_i(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread);
void dconst_i(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread);
void bipush(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread);
void sipush(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread);
void ldc(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread);
void ldc_w(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread);
void ldc2_w(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread);