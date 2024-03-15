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

JCALL void lib_sun_reflect_Reflection_getCallerClass(const NativeArgs& args) {
    ClassInfo* previousClass = args.thread->m_stack.frames[args.thread->m_stack.frames.size()-2].previousClass;
    const u4 ref = args.heap->createClassObject(previousClass, args.vm, previousClass->getName());
    const Variable classRef{VariableType_REFERENCE, ref};
    args.thread->returnVar(classRef);
}

JCALL void lib_sun_reflect_Reflection_getClassAccessFlags(const NativeArgs& args)
{
    const ClassObject* classObject = args.heap->getClassObject(args.thread->m_currentFrame->localVariables[0].data);
    args.thread->returnVar(Variable{VariableType_INT, classObject->classClassInfo->accessFlags});
}
