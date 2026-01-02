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

#include "AccessController.h"

JCALL void lib_java_security_AccessController_doPriviliged(const NativeArgs& args)
{
    const StackFrame* currentFrame = args.thread->m_currentFrame;
    const vdata objectVar = currentFrame->localVariables[0];
    const Object* method = VM::get()->getHeap()->getObject(currentFrame->localVariables[0].getReference());
    const MethodInfo* methodInfo = method->classInfo->findMethodWithNameAndDescriptor("run", "()Ljava/lang/Object;");
    ClassInfo* classInfo = method->classInfo;

    args.thread->pushStackFrameWithoutParams(classInfo, methodInfo);
    args.thread->m_currentFrame->localVariables[0] = objectVar;

    args.thread->executeLoop();
}

JCALL void lib_java_security_AccessController_doPriviliged_PriviligedExceptionAction(const NativeArgs& args)
{
    // TODO: Catch checked exceptions and throw a PrivilegedActionException
    // when exception handling is properly implemented
    const StackFrame* currentFrame = args.thread->m_currentFrame;
    const vdata objectVar = currentFrame->localVariables[0];
    const Object* method = VM::get()->getHeap()->getObject(currentFrame->localVariables[0].getReference());
    const MethodInfo* methodInfo = method->classInfo->findMethodWithNameAndDescriptor("run", "()Ljava/lang/Object;");
    ClassInfo* classInfo = method->classInfo;

    args.thread->pushStackFrameWithoutParams(classInfo, methodInfo);
    args.thread->m_currentFrame->localVariables[0] = objectVar;

    args.thread->executeLoop();
}

JCALL void lib_java_security_AccessController_getStackAccessControlContext(const NativeArgs& args)
{
    args.thread->returnVar(vdata(VariableType_REFERENCE, static_cast<vreference>(0)));
}
