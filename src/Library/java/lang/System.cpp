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

#include "System.h"

#include <cstdint>
#include <chrono>

JCALL void lib_java_lang_System_registerNatives(const NativeArgs& args)
{
    registerNative("java/lang/System/arraycopy", "(Ljava/lang/Object;ILjava/lang/Object;II)V", lib_java_lang_System_arraycopy);
    registerNative("java/lang/System/initProperties", "(Ljava/util/Properties;)Ljava/util/Properties;", lib_java_lang_System_initProperties);
    registerNative("java/lang/System/setIn0", "(Ljava/io/InputStream;)V", lib_java_lang_System_setIn0);
    registerNative("java/lang/System/setOut0", "(Ljava/io/PrintStream;)V", lib_java_lang_System_setOut0);
    registerNative("java/lang/System/setErr0", "(Ljava/io/PrintStream;)V", lib_java_lang_System_setErr0);
    registerNative("java/lang/System/currentTimeMillis", "()J", lib_java_lang_System_currentTimeMillis);
    registerNative("java/lang/System/mapLibraryName", "(Ljava/lang/String;)Ljava/lang/String;", lib_java_lang_System_mapLibraryName);
}

JCALL void lib_java_lang_System_arraycopy(const NativeArgs& args)
{
    const StackFrame* currentFrame = args.thread->m_currentFrame;
    const Variable srcObjectRef = currentFrame->localVariables[0];
    const Variable srcPosVar = currentFrame->localVariables[1];
    const Variable dstObjectRef = currentFrame->localVariables[2];
    const Variable dstPosVar = currentFrame->localVariables[3];
    const Variable lengthVar = currentFrame->localVariables[4];

    const Array* srcArray = args.heap->getArray(srcObjectRef.data);
    const Array* dstArray = args.heap->getArray(dstObjectRef.data);

    // TODO: De-duplicate this code
    u1 bytes = 4;
    if (srcArray->arrayType == AT_CHAR)
    {
        bytes = 2;
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

static void setProperty(const NativeArgs& args, Variable propertiesObjectRef, ClassInfo* classInfo, const MethodInfo* methodInfo, const char* key, const char* value)
{
    args.thread->pushStackFrameWithoutParams(classInfo, methodInfo);
    args.thread->m_currentFrame->localVariables[0] = propertiesObjectRef;
    args.thread->m_currentFrame->localVariables[1] = Variable{VariableType_REFERENCE,args.heap->createString(key, args.vm)};
    args.thread->m_currentFrame->localVariables[2] = Variable{VariableType_REFERENCE,args.heap->createString(value, args.vm)};
    args.vm->executeLoop(args.thread);
}

JCALL void lib_java_lang_System_initProperties(const NativeArgs& args)
{
    const Variable propertiesObjectRef = args.thread->m_currentFrame->localVariables[0];
    const Object* properties = args.heap->getObject(propertiesObjectRef.data);
    const MethodInfo* entryPoint = properties->classInfo->findMethodWithNameAndDescriptor("setProperty", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Object;");

    setProperty(args, propertiesObjectRef, properties->classInfo, entryPoint, "file.encoding", "Cp1252");
    setProperty(args, propertiesObjectRef, properties->classInfo, entryPoint, "user.dir", args.vm->userDir.c_str());
    setProperty(args, propertiesObjectRef, properties->classInfo, entryPoint, "sun.jnu.encoding", "Cp1252");
    setProperty(args, propertiesObjectRef, properties->classInfo, entryPoint, "os.version", "10.0");
    setProperty(args, propertiesObjectRef, properties->classInfo, entryPoint, "file.separator", "\\");
    setProperty(args, propertiesObjectRef, properties->classInfo, entryPoint, "path.separator", ";");
    setProperty(args, propertiesObjectRef, properties->classInfo, entryPoint, "line.separator", "\n");
    setProperty(args, propertiesObjectRef, properties->classInfo, entryPoint, "sun.stdout.encoding", "UTF-8");
    // setProperty(args, propertiesObjectRef, properties->classInfo, entryPoint, "sun.stderr.encoding", "cp850");

    args.thread->returnVar(propertiesObjectRef);
}

JCALL void lib_java_lang_System_setIn0(const NativeArgs& args)
{
    const ClassInfo* classInfo = args.vm->getClass("java/lang/System", args.thread);
    const FieldInfo* field = classInfo->findField("in", "Ljava/io/InputStream;");
    field->staticData->data = args.thread->m_currentFrame->localVariables[0].data;
}

JCALL void lib_java_lang_System_setOut0(const NativeArgs& args)
{
    const ClassInfo* classInfo = args.vm->getClass("java/lang/System", args.thread);
    const FieldInfo* field = classInfo->findField("out", "Ljava/io/PrintStream;");
    field->staticData->data = args.thread->m_currentFrame->localVariables[0].data;
}

JCALL void lib_java_lang_System_setErr0(const NativeArgs& args)
{
    const ClassInfo* classInfo = args.vm->getClass("java/lang/System", args.thread);
    const FieldInfo* field = classInfo->findField("err", "Ljava/io/PrintStream;");
    field->staticData->data = args.thread->m_currentFrame->localVariables[0].data;
}

JCALL void lib_java_lang_System_currentTimeMillis(const NativeArgs &args) {
    using namespace std::chrono;
    const u8 millis = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    const auto parts = reinterpret_cast<const u4*>(&millis);
    args.thread->returnVar(Variable{VariableType_LONG, parts[1]},
        Variable{VariableType_LONG, parts[0]});
    printf("");
}

JCALL void lib_java_lang_System_mapLibraryName(const NativeArgs &args) {
    Variable var = args.thread->m_currentFrame->localVariables[0];
    // TODO: Append '.dll' for Windows
    printf("|String Interned with id: %d\n", var.data);
    args.thread->returnVar(Variable{VariableType_REFERENCE, var.data});
}
