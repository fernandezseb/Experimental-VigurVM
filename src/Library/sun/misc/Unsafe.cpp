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

#include "Unsafe.h"

#include <bit>

JCALL void lib_sun_misc_Unsafe_registerNatives(NATIVE_ARGS)
{
    printf("[Running sun/misc/Unsafe/registerNatives()V]\n");
    registerNative("sun/misc/Unsafe/arrayBaseOffset", "(Ljava/lang/Class;)I", lib_sun_misc_Unsafe_arrayBaseOffset);
    registerNative("sun/misc/Unsafe/arrayIndexScale", "(Ljava/lang/Class;)I", lib_sun_misc_Unsafe_arrayIndexScale);
    registerNative("sun/misc/Unsafe/addressSize", "()I", lib_sun_misc_Unsafe_addressSize);
    registerNative("sun/misc/Unsafe/objectFieldOffset", "(Ljava/lang/reflect/Field;)J", lib_sun_misc_Unsafe_objectFieldOffset);
    registerNative("sun/misc/Unsafe/compareAndSwapObject", "(Ljava/lang/Object;JLjava/lang/Object;Ljava/lang/Object;)Z", lib_sun_misc_Unsafe_compareAndSwapObject);
    registerNative("sun/misc/Unsafe/compareAndSwapInt", "(Ljava/lang/Object;JII)Z", lib_sun_misc_Unsafe_compareAndSwapInt);
    registerNative("sun/misc/Unsafe/getIntVolatile", "(Ljava/lang/Object;J)I", lib_sun_misc_Unsafe_getIntVolatile);
}

JCALL void lib_sun_misc_Unsafe_arrayBaseOffset(NATIVE_ARGS)
{
    constexpr u4 offset = offsetof(Array, data);
    constexpr u4 val = std::bit_cast<u4>(offset);
    thread->returnVar(Variable{VariableType_INT,  val});
}

JCALL void lib_sun_misc_Unsafe_arrayIndexScale(NATIVE_ARGS)
{
    const Variable classObjectRef = thread->m_currentFrame->localVariables[1];
    const ClassObject* classObject = heap->getClassObject(classObjectRef.data);
    u4 length = 4;
    if (classObject->name[1] == 'C')
    {
        length = 1;
    } else if (classObject->name[1] == 'D' || classObject->name[1] == 'J')
    {
        length = 8;
    }
    thread->returnVar(Variable{VariableType_INT, length});
}

JCALL void lib_sun_misc_Unsafe_addressSize(NATIVE_ARGS)
{
    constexpr Variable size{VariableType_INT, 64};
    thread->returnVar(size);
}

JCALL void lib_sun_misc_Unsafe_objectFieldOffset(NATIVE_ARGS)
{
    const Variable fieldObjectRef = thread->m_currentFrame->localVariables[1];
    const Object* fieldObject = heap->getObject(fieldObjectRef.data);
    const FieldData* slotField = fieldObject->getField("slot", "I", heap);
    const u4 slot = slotField->data->data;
    const FieldData* classField = fieldObject->getField("clazz", "Ljava/lang/Class;", heap);
    ClassObject* classObject = heap->getClassObject(classField->data->data);
    u4 index = 0;
    // Ignore static fields
    for (u4 currentField = 0; currentField < slot; ++currentField)
    {
        FieldInfo* fieldInfo = classObject->classClassInfo->fields[currentField];
        if (!fieldInfo->isStatic())
        {
            index++;
        }
    }
    constexpr u4 baseOffset = offsetof(Object, fields);
    const u4 fieldOffset = baseOffset + (index * sizeof(FieldData));
    thread->returnVar(Variable{VariableType_LONG, 0}, Variable{VariableType_LONG, fieldOffset});
}

JCALL void lib_sun_misc_Unsafe_compareAndSwapObject(NATIVE_ARGS)
{
    const Variable oObjectRef = thread->m_currentFrame->localVariables[1];
    const Object* oObject = heap->getObject(oObjectRef.data);
    const u8 offsetVar = ((static_cast<u8>(thread->m_currentFrame->localVariables[2].data) << 32) | static_cast<u8>(thread->m_currentFrame->localVariables[3].data));
    const Variable expectedObjectRef = thread->m_currentFrame->localVariables[4];
    const Variable xObjectRef = thread->m_currentFrame->localVariables[5];
    constexpr u4 baseOffset = offsetof(Object, fields);
    const u4 fieldIndex = (offsetVar-baseOffset)/sizeof(FieldData);
    if (fieldIndex >= oObject->fields.size())
    {
        thread->internalError("Too big offset");
    }
    const FieldData* fieldData = &oObject->fields[fieldIndex];
    if (fieldData->data->data == expectedObjectRef.data)
    {
        fieldData->data->data = xObjectRef.data;
        thread->returnVar(Variable{VariableType_INT, 1u});
    } else
    {
        thread->returnVar(Variable{VariableType_INT, 0u});
    }
}

JCALL void lib_sun_misc_Unsafe_compareAndSwapInt(NATIVE_ARGS)
{
    const Variable oObjectRef = thread->m_currentFrame->localVariables[1];
    const Object* oObject = heap->getObject(oObjectRef.data);
    const u8 offsetVar = ((static_cast<u8>(thread->m_currentFrame->localVariables[2].data) << 32) | static_cast<u8>(thread->m_currentFrame->localVariables[3].data));
    const Variable expectedIntVar = thread->m_currentFrame->localVariables[4];
    const Variable xIntVar = thread->m_currentFrame->localVariables[5];
    constexpr u4 baseOffset = offsetof(Object, fields);
    const u4 fieldIndex = (offsetVar-baseOffset)/sizeof(FieldData);
    if (fieldIndex >= oObject->fields.size())
    {
        thread->internalError("Too big offset");
    }
    const FieldData* fieldData = &oObject->fields[fieldIndex];
    if (fieldData->data->data == expectedIntVar.data)
    {
        fieldData->data->data = xIntVar.data;
        thread->returnVar(Variable{VariableType_INT, 1u});
    } else
    {
        thread->returnVar(Variable{VariableType_INT, 0u});
    }
}

JCALL void lib_sun_misc_Unsafe_getIntVolatile(NATIVE_ARGS)
{
    constexpr u4 baseOffset = offsetof(Object, fields);
    const Variable oObjectRef = thread->m_currentFrame->localVariables[1];
    const Object* oObject = heap->getObject(oObjectRef.data);
    const u8 offset = ((static_cast<u8>(thread->m_currentFrame->localVariables[2].data) << 32) | static_cast<u8>(thread->m_currentFrame->localVariables[3].data));
    const u4 fieldOffset = (offset-baseOffset)/sizeof(FieldData);
    const FieldData fieldData = oObject->fields[fieldOffset];

    thread->returnVar(Variable{VariableType_INT, fieldData.data->data});
}
