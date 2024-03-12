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

#include "System.h"

JCALL void lib_java_lang_System_registerNatives(NATIVE_ARGS)
{
    registerNative("java/lang/System/arraycopy", "(Ljava/lang/Object;ILjava/lang/Object;II)V", lib_java_lang_System_arraycopy);
    registerNative("java/lang/System/initProperties", "(Ljava/util/Properties;)Ljava/util/Properties;", lib_java_lang_System_initProperties);
    registerNative("java/lang/System/setIn0", "(Ljava/io/InputStream;)V", lib_java_lang_System_setIn0);
}

JCALL void lib_java_lang_System_arraycopy(NATIVE_ARGS)
{
    const StackFrame* currentFrame = thread->m_currentFrame;
    const Variable srcObjectRef = currentFrame->localVariables[0];
    const Variable srcPosVar = currentFrame->localVariables[1];
    const Variable dstObjectRef = currentFrame->localVariables[2];
    const Variable dstPosVar = currentFrame->localVariables[3];
    const Variable lengthVar = currentFrame->localVariables[4];

    const Array* srcArray = heap->getArray(srcObjectRef.data);
    const Array* dstArray = heap->getArray(dstObjectRef.data);

    // TODO: De-duplicate this code
    u1 bytes = 4;
    if (srcArray->arrayType == AT_CHAR)
    {
        bytes = 1;
    }
    else if (srcArray->arrayType == AT_LONG || srcArray->arrayType == AT_DOUBLE)
    {
        bytes = 8;
    }

    // TODO: Do checks before copying

    memcpy(dstArray->data+(dstPosVar.data*bytes),
        srcArray->data+(srcPosVar.data*bytes),
        lengthVar.data * bytes);
}

JCALL void lib_java_lang_System_initProperties(NATIVE_ARGS)
{
    const Variable propertiesObjectRef = thread->m_currentFrame->localVariables[0];
    const Object* properties = heap->getObject(propertiesObjectRef.data);
    const MethodInfo* entryPoint = properties->classInfo->findMethodWithNameAndDescriptor("setProperty", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Object;");

    thread->pushStackFrameWithoutParams(properties->classInfo, entryPoint);
    thread->m_currentFrame->localVariables[0] = propertiesObjectRef;
    thread->m_currentFrame->localVariables[1] = Variable{VariableType_REFERENCE,heap->createString("file.encoding", VM)};
    thread->m_currentFrame->localVariables[2] = Variable{VariableType_REFERENCE,heap->createString("UTF-8", VM)};
    VM->executeLoop(thread);

    thread->returnVar(propertiesObjectRef);
}

JCALL void lib_java_lang_System_setIn0(NATIVE_ARGS)
{
    const ClassInfo* classInfo = VM->getClass("java/lang/System", thread);
    const FieldInfo* field = classInfo->findField("in", "Ljava/io/InputStream;");
    field->staticData->data = thread->m_currentFrame->localVariables[0].data;
}
