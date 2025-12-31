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
    const vdata srcObjectRef = currentFrame->localVariables[0];
    const vdata srcPosVar = currentFrame->localVariables[1];
    const vdata dstObjectRef = currentFrame->localVariables[2];
    const vdata dstPosVar = currentFrame->localVariables[3];
    const vdata lengthVar = currentFrame->localVariables[4];

    const Array* srcArray = VM::get()->getHeap()->getArray(srcObjectRef.getReference());
    const Array* dstArray = VM::get()->getHeap()->getArray(dstObjectRef.getReference());

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

    memcpy(dstArray->data+(dstPosVar.getInt()*bytes),
        srcArray->data+(srcPosVar.getInt()*bytes),
        lengthVar.getInt() * bytes);
}

static void setProperty(const NativeArgs& args, vdata propertiesObjectRef, ClassInfo* classInfo, const MethodInfo* methodInfo, const char* key, const char* value)
{
    args.thread->pushStackFrameWithoutParams(classInfo, methodInfo);
    args.thread->m_currentFrame->localVariables[0] = propertiesObjectRef;
    args.thread->m_currentFrame->localVariables[1] = vdata(VariableType_REFERENCE,VM::get()->getHeap()->createString(key));
    args.thread->m_currentFrame->localVariables[2] = vdata(VariableType_REFERENCE,VM::get()->getHeap()->createString(value));
    args.thread->executeLoop();
}

JCALL void lib_java_lang_System_initProperties(const NativeArgs& args)
{
    const vdata propertiesObjectRef = args.thread->m_currentFrame->localVariables[0];
    const Object* properties = VM::get()->getHeap()->getObject(propertiesObjectRef.getReference());
    const MethodInfo* entryPoint = properties->classInfo->findMethodWithNameAndDescriptor("setProperty", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Object;");

    setProperty(args, propertiesObjectRef, properties->classInfo, entryPoint, "file.encoding", "Cp1252");
    setProperty(args, propertiesObjectRef, properties->classInfo, entryPoint, "user.dir", VM::get()->userDir.c_str());
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
    const ClassInfo* classInfo = args.thread->getClass("java/lang/System");
    FieldInfo* field = classInfo->findField("in", "Ljava/io/InputStream;");
    field->staticData.value.l = args.thread->m_currentFrame->localVariables[0].getReference();
}

JCALL void lib_java_lang_System_setOut0(const NativeArgs& args)
{
    const ClassInfo* classInfo = args.thread->getClass("java/lang/System");
    FieldInfo* field = classInfo->findField("out", "Ljava/io/PrintStream;");
    field->staticData.value.l = args.thread->m_currentFrame->localVariables[0].getReference();
}

JCALL void lib_java_lang_System_setErr0(const NativeArgs& args)
{
    const ClassInfo* classInfo = args.thread->getClass("java/lang/System");
    FieldInfo* field = classInfo->findField("err", "Ljava/io/PrintStream;");
    field->staticData.value.l = args.thread->m_currentFrame->localVariables[0].getReference();
}

JCALL void lib_java_lang_System_currentTimeMillis(const NativeArgs &args) {
    using namespace std::chrono;
    const u8 millis = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    const vlong time = millis;
    args.thread->returnVar(vdata(VariableType_LONG, time));
    printf("");
}

JCALL void lib_java_lang_System_mapLibraryName(const NativeArgs &args) {
    vdata var = args.thread->m_currentFrame->localVariables[0];
    // TODO: Append '.dll' for Windows
    args.thread->returnVar(vdata{VariableType_REFERENCE, var.getReference()});
}
