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

#include "VM.h"

#include "Error.h"
#include "Configuration.h"
#include "Memory.h"
#include "Library/Builtin.h"
#include "physfs.h"

#include <string>
#include <variant>
#include <utility>

VM* VM::m_vm = nullptr;

VM* VM::create(const Configuration config)
{
    m_vm = new VM(config);
    return m_vm;
}

VM* VM::get()
{
    return m_vm;
}

VM::VM(const Configuration configuration) noexcept
    : m_bootstrapClassLoader(), m_configuration(configuration)
{
}

void VM::start(std::string_view commandLineName)
{
    Platform::initialize();
    PHYSFS_init(commandLineName.data());
    PHYSFS_permitSymbolicLinks(0);

    const size_t backSlash = commandLineName.find_last_of('\\');
    const size_t forwardSlash = commandLineName.find_last_of('/');
    userDir = commandLineName.substr(0, backSlash < forwardSlash ? backSlash : forwardSlash);
    PHYSFS_addToSearchPath(userDir.c_str(), 1);

    m_threads.emplace_back("main", 200);
    VMThread* mainThread = &m_threads[0];

    registerBuiltinRegisterNatives();
    mainThread->getClass("java/lang/OutOfMemoryError");
    mainThread->getClass("java/lang/VirtualMachineError");
    mainThread->getClass("java/lang/Object");
    mainThread->getClass("java/lang/Number");
    ClassInfo* classInfo = mainThread->getClass("java/lang/Class");
    m_heap.setClassInfo(classInfo);
    mainThread->getClass("java/lang/String");
    ClassInfo* systemClass = mainThread->getClass("java/lang/System");
    mainThread->getClass("java/lang/Thread");
    mainThread->getClass("java/lang/ThreadGroup");
    mainThread->getClass("java/lang/reflect/Field");
    mainThread->getClass("java/lang/reflect/AccessibleObject");
    mainThread->getClass("java/lang/reflect/Executable");
    mainThread->getClass("java/lang/reflect/Constructor");
    mainThread->getClass("java/io/PrintStream");
    mainThread->getClass("java/io/FilterOutputStream");
    mainThread->getClass("java/io/OutputStream");
    mainThread->getClass("java/util/Properties");

    const u4 threadGroupReference = createThreadGroupObject(mainThread);
    mainThread->threadObject = createThreadObject(mainThread, threadGroupReference);

    if (!m_configuration.m_disableSystemInit)
    {
        initSystemClass(systemClass, mainThread);
    }
}

u4 VM::createThreadGroupObject(VMThread* thread)
{
    ClassInfo* threadGroupClass = thread->getClass("java/lang/ThreadGroup");
    const u4 threadGroupReference = m_heap.createObject(threadGroupClass);
    const Object* threadGroupObject = m_heap.getObject(threadGroupReference);

    FieldData* maxPrioField = threadGroupObject->getField("maxPriority", "I");
    maxPrioField->value.i = 10;


    return threadGroupReference;
}

u4 VM::createThreadObject(VMThread* thread, const u4 threadGroupReference)
{
    ClassInfo* threadClass = thread->getClass("java/lang/Thread");
    const u4 objectReference = m_heap.createObject(threadClass);
    const Object* threadObject = m_heap.getObject(objectReference);

    FieldData* groupField = threadObject->getField("group", "Ljava/lang/ThreadGroup;");
    groupField->value.l = threadGroupReference;

    FieldData* priorityField = threadObject->getField("priority", "I");
    priorityField->value.i = thread->priority;

    return objectReference;
}

// TODO: Refactor this with simpler creation
vdata VM::createVariableForDescriptor(std::string_view descriptor)
{
    if (descriptor == "I")
    {
        return vdata(VariableType_INT);
    } else if (descriptor ==  "Z")
    {
        return vdata(VariableType_INT);
    } else if (descriptor == "B")
    {
        return vdata(VariableType_INT);
    } else if (descriptor == "J")
    {
        return vdata(VariableType_LONG);
    } else if (descriptor ==  "D")
    {
        return vdata(VariableType_DOUBLE);
    } else if (descriptor[0] == '[')
    {
        return vdata(VariableType_REFERENCE);
    } else if (descriptor[0] == 'L')
    {
        return vdata(VariableType_REFERENCE);
    } else if (descriptor[0] == 'F') {
        return vdata(VariableType_FLOAT);
    } else if (descriptor[0] == 'C')
    {
        return vdata(VariableType_INT);
    }
    else
    {
        fprintf(stderr, "Error: Couldn't construct data for descriptor type: %s\n", descriptor.data());
        Platform::exitProgram(7);
        std::unreachable();
    }
}


JavaHeap* VM::getHeap()
{
    return &m_heap;
}


void VM::updateVariableFromVariable(vdata* variable, std::string_view descriptor, vdata operand, VMThread* thread)
{
    if (descriptor ==  "I")
    {
        variable->checkType(VariableType_INT);
        operand.checkType(VariableType_INT);

        variable->value.i = operand.value.i;
    } else if (descriptor ==  "C")
    {
        variable->checkType(VariableType_INT);
        // checkType(operand, VariableType_CHAR, thread); TODO: Enable

        variable->value.i = operand.value.i;
    } else if (descriptor ==  "Z")
    {
        variable->checkType(VariableType_INT);

        variable->value.i = operand.value.i;
    } else if (descriptor[0] == '[' || descriptor[0] == 'L') {

        variable->checkType(VariableType_REFERENCE);
        operand.checkType(VariableType_REFERENCE);

        variable->value.l = operand.value.l;
    } else if (descriptor[0] == 'J')
    {
        variable->checkType(VariableType_LONG);
        operand.checkType(VariableType_LONG);
        variable->value.j = operand.value.j;
    } else if (descriptor[0] == 'D')
    {
        variable->checkType(VariableType_DOUBLE);
        operand.checkType(VariableType_DOUBLE);
        variable->value.d = operand.value.d;
    } else
    {
        char buffer[200];
        snprintf(buffer, 200, "Error: Setting of variable of type with descriptor: %s not implemented yet!\n", descriptor.data());
        thread->internalError(buffer, 5);
    }
}

void VM::createArgsArray(const VMThread* thread)
{
    const u4 arrayRef =  m_heap.createArray(AT_REFERENCE, m_configuration.m_args.size(), "Ljava/lang/String;");
    const Array* array = m_heap.getArray(arrayRef);
    u4* arrayData = reinterpret_cast<u4*>(array->data);
    u4 currentArg = 0;
    for (std::string_view arg : m_configuration.m_args)
    {
        const u4 stringRef = m_heap.createString(arg.data());
        arrayData[currentArg++] = stringRef;
    }
    thread->m_currentFrame->localVariables[0] = vdata{VariableType_REFERENCE, arrayRef};

}

void VM::runMain()
{
    VMThread* mainThread = getCurrentVMThread();
    if (m_configuration.m_mainClassName.empty())
    {
        fprintf(stderr, "Error: Class name of starting class not defined..\n");
        Platform::exitProgram(6);
    }

    ClassInfo* startupClass = mainThread->getClass(m_configuration.m_mainClassName.data());
    MethodInfo* entryPoint = startupClass->findMethodWithNameAndDescriptor("main", "([Ljava/lang/String;)V");

    if (entryPoint == nullptr)
    {
        fprintf(stderr, "Error: Entry point not found. Exiting...\n");
        Platform::exitProgram(6);
    }

    mainThread->m_pc = 0;
    mainThread->m_currentClass = startupClass;
    mainThread->m_currentMethod = entryPoint;

    mainThread->pushStackFrameStatic(startupClass, entryPoint, 0);

    createArgsArray(mainThread);

    // printf("> Executing main method...\n");
    mainThread->executeLoop();
    // Object* object = heap.getObject(3);
    // printf("> Done executing\n");
}

void VM::initSystemClass(ClassInfo* systemClass, VMThread* thread)
{
    MethodInfo* initMethod = systemClass->findMethodWithNameAndDescriptor("initializeSystemClass", "()V");

    thread->m_pc = 0;
    thread->m_currentClass = systemClass;
    thread->m_currentMethod = initMethod;

    thread->pushStackFrameSpecial(systemClass, initMethod, nullptr);

    // printf("> Executing System init method...\n");
    thread->executeLoop();
    // printf("> Done executing System init method\n");
}

void VM::shutdown()
{
    PHYSFS_deinit();
    Platform::cleanup();
}

VMThread* VM::getVMThreadByObjectRef(const vreference objectReference)
{
    for (VMThread& thread : m_threads)
    {
        if (thread.threadObject == objectReference)
        {
            return &thread;
        }
    }

    return nullptr;
}

VMThread* VM::getCurrentVMThread()
{
    for (VMThread& thread : m_threads)
    {
        if (thread.m_pthreadId == std::this_thread::get_id())
        {
            return &thread;
        }
    }

    return nullptr;
}

