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
    StackFrame* currentFrame = thread->m_currentFrame;
    Variable objectVar = currentFrame->localVariables[0];
    Object* method = heap->getObject(currentFrame->localVariables[0].data);
    MethodInfo* methodInfo = method->classInfo->findMethodWithNameAndDescriptor("run", "()Ljava/lang/Object;");
    ClassInfo* classInfo = method->classInfo;

    const u4 oldPc = thread->m_pc;
    ClassInfo* oldCurrentClass = thread->m_currentClass;
    const MethodInfo* oldCurrentMethod = thread->m_currentMethod;
    StackFrame* oldFrame = thread->m_currentFrame;
    thread->m_stackstack.emplace_back(200);

    thread->pushStackFrameWithoutParams(classInfo, methodInfo);

    thread->m_currentFrame->localVariables[0] = objectVar;

    VM->executeLoop(thread);

    thread->m_pc = oldPc;
    thread->m_currentClass = oldCurrentClass;
    thread->m_currentMethod = oldCurrentMethod;
    thread->m_stackstack.pop_back();
    thread->m_currentFrame = oldFrame;
}
