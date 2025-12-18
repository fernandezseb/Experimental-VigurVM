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

#include "NativeConstructorAccessorImpl.h"

JCALL void lib_sun_reflect_NativeConstructorAccessorImpl_newInstance0(const NativeArgs& args)
{
    const Object* constructor = args.heap->getObject(args.thread->m_currentFrame->localVariables[0].data);

    const FieldData* slotField = constructor->getField("slot", "I", args.heap);

    // Set the class field
    const FieldData* classField = constructor->getField("clazz", "Ljava/lang/Class;", args.heap);
    const ClassObject* classObject = args.heap->getClassObject(classField->data);

    // Set the modifiers field
    const FieldData* modifiersField = constructor->getField("modifiers", "I", args.heap);

    // Set the parameterTypes field
    const FieldData* parameterTypes = constructor->getField("parameterTypes", "[Ljava/lang/Class;", args.heap);


    const MethodInfo* constructorMethod = classObject->classClassInfo->methods[slotField->data];


    const u4 objectRef = args.heap->createObject(classObject->classClassInfo, args.vm);

    args.thread->pushStackFrameWithoutParams(classObject->classClassInfo, constructorMethod);
    args.thread->m_currentFrame->localVariables[0] = Variable{VariableType_REFERENCE, objectRef};
    // TODO: Implement passing of n arguments to constructor

    args.vm->executeLoop(args.thread);

    args.thread->returnVar(Variable{VariableType_REFERENCE,  objectRef});


    printf("");
}
