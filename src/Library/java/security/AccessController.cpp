/*
 * Copyright (c) 2023-2024 Sebastiaan Fernandez.
 *
 * This file is part of VigurVM.
 *
 * VigurVM is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * VigurVM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with Foobar.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "AccessController.h"

void lib_java_security_AccessController_doPriviliged(const NativeArgs& args)
{
    const StackFrame* currentFrame = args.thread->m_currentFrame;
    const Variable objectVar = currentFrame->localVariables[0];
    const Object* method = args.heap->getObject(currentFrame->localVariables[0].data);
    const MethodInfo* methodInfo = method->classInfo->findMethodWithNameAndDescriptor("run", "()Ljava/lang/Object;");
    ClassInfo* classInfo = method->classInfo;

    args.thread->pushStackFrameWithoutParams(classInfo, methodInfo);
    args.thread->m_currentFrame->localVariables[0] = objectVar;

    args.vm->executeLoop(args.thread);
}

void lib_java_security_AccessController_doPriviliged_PriviligedExceptionAction(const NativeArgs& args)
{
    // TODO: Catch checked exceptions and throw a PrivilegedActionException
    // when exception handling is properly implemented
    const StackFrame* currentFrame = args.thread->m_currentFrame;
    const Variable objectVar = currentFrame->localVariables[0];
    const Object* method = args.heap->getObject(currentFrame->localVariables[0].data);
    const MethodInfo* methodInfo = method->classInfo->findMethodWithNameAndDescriptor("run", "()Ljava/lang/Object;");
    ClassInfo* classInfo = method->classInfo;

    args.thread->pushStackFrameWithoutParams(classInfo, methodInfo);
    args.thread->m_currentFrame->localVariables[0] = objectVar;

    args.vm->executeLoop(args.thread);
}

void lib_java_security_AccessController_getStackAccessControlContext(const NativeArgs& args)
{
    args.thread->returnVar(Variable{VariableType_REFERENCE, 0});
}
