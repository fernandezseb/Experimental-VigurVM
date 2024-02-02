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

void lib_java_security_AccessController_doPriviliged(NATIVE_ARGS)
{
    const StackFrame* currentFrame = thread->m_currentFrame;
    const Variable objectVar = currentFrame->localVariables[0];
    const Object* method = heap->getObject(currentFrame->localVariables[0].data);
    const MethodInfo* methodInfo = method->classInfo->findMethodWithNameAndDescriptor("run", "()Ljava/lang/Object;");
    ClassInfo* classInfo = method->classInfo;

    thread->pushStackFrameWithoutParams(classInfo, methodInfo);
    thread->m_currentFrame->localVariables[0] = objectVar;

    VM->executeLoop(thread);
}

void lib_java_security_AccessController_getStackAccessControlContext(NATIVE_ARGS)
{
    thread->returnVar(Variable{VariableType_REFERENCE, 0});
}
