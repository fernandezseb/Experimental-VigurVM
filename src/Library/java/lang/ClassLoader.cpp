/*
 * Copyright (c) 2023-2025 Sebastiaan Fernandez.
 *
 * This file is part of VigurVM.
 *
 * VigurVM is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * VigurVM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with VigurVM.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "ClassLoader.h"
#include "Data/Descriptor.h"

#include <algorithm>
#include <string>

#include "Error.h"

JCALL void lib_java_lang_ClassLoader_registerNatives(const NativeArgs& args)
{
    // registerNative("java/lang/Class/getPrimitiveClass", "(Ljava/lang/String;)Ljava/lang/Class;",
    //                lib_java_lang_Class_getPrimitiveClass);
    registerNative("java/lang/ClassLoader/findBuiltinLib", "(Ljava/lang/String;)Ljava/lang/String;", lib_java_lang_ClassLoader_findBuiltinLib);
    registerNative("java/lang/ClassLoader$NativeLibrary/load", "(Ljava/lang/String;Z)V", lib_java_lang_ClassLoader$NativeLibrary_load);
}

JCALL void lib_java_lang_ClassLoader_findBuiltinLib(const NativeArgs& args)
{
    VMThread* thread = args.thread;
    const VM* VM = args.vm;
    const StackFrame* currentFrame = thread->m_currentFrame;
    const Variable var = currentFrame->localVariables[0];
    VM->checkType(var, VariableType_REFERENCE, thread);
    args.thread->returnVar(Variable{VariableType_REFERENCE, var.data});
}

JCALL void lib_java_lang_ClassLoader$NativeLibrary_load(const NativeArgs& args)
{
    Variable builtin = args.thread->m_currentFrame->localVariables[2];
    if (builtin.data)
    {
        // TODO: Set the loaded boolean of the NativeLibrary instance to true
        const Object* object = args.getThisObjectReference();
        FieldData& data = object->fields[5];
        data.data = 1;
        printf("");
    } else
    {
        // TODO: Load library
        args.thread->internalError("Not implemented yet", ErrorCode::NOT_IMPLEMENTED_YET);
    }
}
