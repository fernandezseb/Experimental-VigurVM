#pragma once

#include "Core.h"

class VMThread;
class JavaHeap;

void astore_i(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread);