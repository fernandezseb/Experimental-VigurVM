#pragma once

#include "Library/NativeDefs.h"

JCALL void lib_java_security_AccessController_doPriviliged(const NativeArgs& args);
JCALL void lib_java_security_AccessController_doPriviliged_PriviligedExceptionAction(const NativeArgs& args);
JCALL void lib_java_security_AccessController_getStackAccessControlContext(const NativeArgs& args);