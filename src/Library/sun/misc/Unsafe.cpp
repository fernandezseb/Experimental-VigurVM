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

#include "Unsafe.h"

#include <bit>
#include <cstdlib>

JCALL void lib_sun_misc_Unsafe_registerNatives([[maybe_unused]]const NativeArgs& args)
{
    // printf("[Running sun/misc/Unsafe/registerNatives()V]\n");
    registerNative("sun/misc/Unsafe/arrayBaseOffset", "(Ljava/lang/Class;)I", lib_sun_misc_Unsafe_arrayBaseOffset);
    registerNative("sun/misc/Unsafe/arrayIndexScale", "(Ljava/lang/Class;)I", lib_sun_misc_Unsafe_arrayIndexScale);
    registerNative("sun/misc/Unsafe/addressSize", "()I", lib_sun_misc_Unsafe_addressSize);
    registerNative("sun/misc/Unsafe/objectFieldOffset", "(Ljava/lang/reflect/Field;)J", lib_sun_misc_Unsafe_objectFieldOffset);
    registerNative("sun/misc/Unsafe/compareAndSwapObject", "(Ljava/lang/Object;JLjava/lang/Object;Ljava/lang/Object;)Z", lib_sun_misc_Unsafe_compareAndSwapObject);
    registerNative("sun/misc/Unsafe/compareAndSwapInt", "(Ljava/lang/Object;JII)Z", lib_sun_misc_Unsafe_compareAndSwapInt);
    registerNative("sun/misc/Unsafe/getIntVolatile", "(Ljava/lang/Object;J)I", lib_sun_misc_Unsafe_getIntVolatile);
    registerNative("sun/misc/Unsafe/allocateMemory", "(J)J", lib_sun_misc_Unsafe_allocateMemory);
    registerNative("sun/misc/Unsafe/freeMemory", "(J)V", lib_sun_misc_Unsafe_freeMemory);
    registerNative("sun/misc/Unsafe/putLong", "(JJ)V", lib_sun_misc_Unsafe_putLong);
    registerNative("sun/misc/Unsafe/getByte", "(J)B", lib_sun_misc_Unsafe_getByte);
}

JCALL void lib_sun_misc_Unsafe_arrayBaseOffset(const NativeArgs& args)
{
    constexpr u4 offset = offsetof(Array, data);
    constexpr vint val = std::bit_cast<vint>(offset);
    args.thread->returnVar(vdata{VariableType_INT,  val});
}

JCALL void lib_sun_misc_Unsafe_arrayIndexScale(const NativeArgs& args)
{
    const vdata classObjectRef = args.thread->m_currentFrame->localVariables[1];
    const ClassObject* classObject = VM::get()->getHeap()->getClassObject(classObjectRef.getReference());
    vint length = 4;
    if (classObject->name[1] == 'C')
    {
        length = 2;
    } else if (classObject->name[1] == 'D' || classObject->name[1] == 'J')
    {
        length = 8;
    }
    args.thread->returnVar(vdata{VariableType_INT, length});
}

JCALL void lib_sun_misc_Unsafe_addressSize(const NativeArgs& args)
{
    const vdata size{VariableType_INT, 64};
    args.thread->returnVar(size);
}

JCALL void lib_sun_misc_Unsafe_objectFieldOffset(const NativeArgs& args)
{
    const vdata fieldObjectRef = args.thread->m_currentFrame->localVariables[1];
    const Object* fieldObject = VM::get()->getHeap()->getObject(fieldObjectRef.getReference());
    const FieldData* slotField = fieldObject->getField("slot", "I");
    const u4 slot = slotField->value.i;
    const FieldData* classField = fieldObject->getField("clazz", "Ljava/lang/Class;");
    ClassObject* classObject = VM::get()->getHeap()->getClassObject(classField->value.l);
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
    args.thread->returnVar(vdata{VariableType_LONG, static_cast<vlong>(fieldOffset)});
}

JCALL void lib_sun_misc_Unsafe_compareAndSwapObject(const NativeArgs& args)
{
    const vdata oObjectRef = args.thread->m_currentFrame->localVariables[1];
    const Object* oObject = VM::get()->getHeap()->getObject(oObjectRef.getReference());
    const u8 offsetVar = static_cast<u8>(args.thread->m_currentFrame->localVariables[2].getLong());
    const vdata expectedObjectRef = args.thread->m_currentFrame->localVariables[4];
    const vdata xObjectRef = args.thread->m_currentFrame->localVariables[5];
    constexpr u4 baseOffset = offsetof(Object, fields);
    const u4 fieldIndex = (offsetVar-baseOffset)/sizeof(FieldData);
    if (fieldIndex >= oObject->fields.size())
    {
        args.thread->internalError("Too big offset", 78);
    }
    FieldData* fieldData = &oObject->fields[fieldIndex];
    if (fieldData->value.l == expectedObjectRef.getReference())
    {
        fieldData->value.l = xObjectRef.getReference();
        args.thread->returnVar(vdata{VariableType_INT, 1});
    } else
    {
        args.thread->returnVar(vdata{VariableType_INT, 0});
    }
}

JCALL void lib_sun_misc_Unsafe_compareAndSwapInt(const NativeArgs& args)
{
    const vdata oObjectRef = args.thread->m_currentFrame->localVariables[1];
    const Object* oObject = VM::get()->getHeap()->getObject(oObjectRef.getReference());
    const u8 offsetVar = static_cast<u8>(args.thread->m_currentFrame->localVariables[2].getLong());
    const vdata expectedIntVar = args.thread->m_currentFrame->localVariables[4];
    const vdata xIntVar = args.thread->m_currentFrame->localVariables[5];
    constexpr u4 baseOffset = offsetof(Object, fields);
    const u4 fieldIndex = (offsetVar-baseOffset)/sizeof(FieldData);
    if (fieldIndex >= oObject->fields.size())
    {
        args.thread->internalError("Too big offset", -9);
    }
    FieldData* fieldData = &oObject->fields[fieldIndex];
    if (fieldData->value.i == expectedIntVar.getInt())
    {
        fieldData->value.i = xIntVar.getInt();
        args.thread->returnVar(vdata{VariableType_INT, 1});
    } else
    {
        args.thread->returnVar(vdata{VariableType_INT, 0});
    }
}

JCALL void lib_sun_misc_Unsafe_getIntVolatile(const NativeArgs& args)
{
    constexpr u4 baseOffset = offsetof(Object, fields);
    const vdata oObjectRef = args.thread->m_currentFrame->localVariables[1];
    const Object* oObject = VM::get()->getHeap()->getObject(oObjectRef.getReference());
    const u8 offset = static_cast<u8>(args.thread->m_currentFrame->localVariables[2].getLong());
    const u4 fieldOffset = (offset-baseOffset)/sizeof(FieldData);
    const FieldData fieldData = oObject->fields[fieldOffset];

    args.thread->returnVar(vdata(VariableType_INT, fieldData.value.i));
}

JCALL void lib_sun_misc_Unsafe_allocateMemory(const NativeArgs& args)
{
    const u8 size = static_cast<u8>(args.thread->m_currentFrame->localVariables[1].getLong());
    const u8 address = reinterpret_cast<const u8> (malloc(size));
    args.thread->returnVar(vdata(VariableType_LONG, static_cast<vlong>(address)));
}

JCALL void lib_sun_misc_Unsafe_freeMemory(const NativeArgs& args)
{
    const u8 address = static_cast<u8>(args.thread->m_currentFrame->localVariables[1].getLong());
    if (address != 0)
    {
        free(reinterpret_cast<void*>(address));
    }
}

JCALL void lib_sun_misc_Unsafe_putLong(const NativeArgs& args)
{
    const u8 addr = static_cast<u8>(args.thread->m_currentFrame->localVariables[1].getLong());
    const u8 x = static_cast<u8>(args.thread->m_currentFrame->localVariables[3].getLong());

    const auto memLocation = reinterpret_cast<u8*>(addr);
    *memLocation = x;
}

JCALL void lib_sun_misc_Unsafe_getByte(const NativeArgs& args)
{
    const vlong addr = args.thread->m_currentFrame->localVariables[1].getLong();
    const auto memLocation = reinterpret_cast<i1*>(addr);

    const i1 val = *memLocation;
    const i4 valExtended = val;
    args.thread->returnVar(vdata(VariableType_INT, valExtended));
}
