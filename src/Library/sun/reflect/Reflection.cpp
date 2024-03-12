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

#include "Reflection.h"

JCALL void lib_sun_reflect_Reflection_getCallerClass(NATIVE_ARGS) {
    ClassInfo* previousClass = thread->m_stack.frames[thread->m_stack.frames.size()-2].previousClass;
    const u4 ref = heap->createClassObject(previousClass, VM, previousClass->getName());
    const Variable classRef{VariableType_REFERENCE, ref};
    thread->returnVar(classRef);
}

JCALL void lib_sun_reflect_Reflection_getClassAccessFlags(NATIVE_ARGS)
{
    const ClassObject* classObject = heap->getClassObject(thread->m_currentFrame->localVariables[0].data);
    thread->returnVar(Variable{VariableType_INT, classObject->classClassInfo->accessFlags});
}
