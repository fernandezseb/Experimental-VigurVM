#pragma once

#include "VM/Native.h"

#define JCALL extern "C"

#define NATIVE_ARGS \
[[maybe_unused]] JavaHeap* heap, \
[[maybe_unused]] VMThread* thread, \
[[maybe_unused]] VM* VM