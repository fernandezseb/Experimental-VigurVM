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
    maxPrioField->data = 10;


    return threadGroupReference;
}

bool VM::isSubclass(VMThread* thread, const ClassInfo* targetClass, ClassInfo* subClass)
{
    ClassInfo* currentClass = subClass;

    while (currentClass != nullptr)
    {
        if (currentClass->getName() == targetClass->getName())
        {
            return true;
        }

        if (currentClass->superClass == 0)
        {
            return false;
        }

        CPClassInfo* classInfo = currentClass->constantPool->getClassInfo(currentClass->superClass);
        currentClass = thread->getClass(currentClass->constantPool->getString(classInfo->nameIndex));
    }

    return false;
}

u4 VM::createThreadObject(VMThread* thread, const u4 threadGroupReference)
{
    ClassInfo* threadClass = thread->getClass("java/lang/Thread");
    const u4 objectReference = m_heap.createObject(threadClass);
    const Object* threadObject = m_heap.getObject(objectReference);

    FieldData* groupField = threadObject->getField("group", "Ljava/lang/ThreadGroup;");
    groupField->data = threadGroupReference;

    FieldData* priorityField = threadObject->getField("priority", "I");
    priorityField->data = thread->priority;

    return objectReference;
}

std::vector<Variable> VM::createVariableForDescriptor(std::string_view descriptor)
{
    std::vector<Variable> variables;
    if (descriptor == "I")
    {
        constexpr Variable variable{VariableType_INT};
        variables.push_back(variable);
    } else if (descriptor ==  "Z")
    {
        constexpr Variable variable{VariableType_INT};
        variables.push_back(variable);
    } else if (descriptor == "B")
    {
        constexpr Variable variable{VariableType_INT};
        variables.push_back(variable);
    } else if (descriptor == "J")
    {
        constexpr Variable variableLow{VariableType_LONG};
        constexpr Variable variableHigh{VariableType_LONG};
        variables.push_back(variableHigh);
        variables.push_back(variableLow);
    } else if (descriptor ==  "D")
    {
        constexpr Variable variableLow{VariableType_DOUBLE};
        constexpr Variable variableHigh{VariableType_DOUBLE};
        variables.push_back(variableHigh);
        variables.push_back(variableLow);
    } else if (descriptor[0] == '[')
    {
        constexpr Variable variable{VariableType_REFERENCE};
        variables.push_back(variable);
    } else if (descriptor[0] == 'L')
    {
        constexpr Variable variable{VariableType_REFERENCE};
        variables.push_back(variable);
    } else if (descriptor[0] == 'F') {
        constexpr Variable variable{VariableType_FLOAT};
        variables.push_back(variable);
    } else if (descriptor[0] == 'C')
    {
        constexpr Variable variable{VariableType_INT};
        variables.push_back(variable);
    }
    else
    {
        fprintf(stderr, "Error: Couldn't construct data for descriptor type: %s\n", descriptor.data());
        Platform::exitProgram(7);
    }
    return variables;
}

u1 VM::getDescriptorVarCategory(std::string_view descriptor) noexcept
{
    // Longs and doubles use two
    if (descriptor[0] == 'J' || descriptor[0] == 'D')
    {
        return 2;
    }
    return 1;
}

JavaHeap* VM::getHeap()
{
    return &m_heap;
}


void VM::updateVariableFromVariable(Variable* variable, std::string_view descriptor, Variable operand, Variable operand2, VMThread* thread)
{
    if (descriptor ==  "I")
    {
        checkType(*variable, VariableType_INT, thread);
        checkType(operand, VariableType_INT, thread);

        variable->data = operand.data;
    } else if (descriptor ==  "C")
    {
        checkType(*variable, VariableType_INT, thread);
        // checkType(operand, VariableType_CHAR, thread);

        variable->data = operand.data;
    } else if (descriptor ==  "Z")
    {
        checkType(*variable, VariableType_INT, thread);

        variable->data = operand.data;
    } else if (descriptor[0] == '[' || descriptor[0] == 'L') {

        checkType(*variable, VariableType_REFERENCE, thread);
        checkType(operand, VariableType_REFERENCE, thread);

        variable->data = operand.data;
    } else if (descriptor[0] == 'J')
    {
        checkType(*variable, VariableType_LONG, thread);
        checkType(operand, VariableType_LONG, thread);
        variable[0].data = operand2.data;
        variable[1].data = operand.data;
    } else if (descriptor[0] == 'D')
    {
        checkType(*variable, VariableType_DOUBLE, thread);
        checkType(operand, VariableType_DOUBLE, thread);
        variable[0].data = operand2.data;
        variable[1].data = operand.data;
    } else
    {
        char buffer[200];
        snprintf(buffer, 200, "Error: Setting of variable of type with descriptor: %s not implemented yet!\n", descriptor.data());
        thread->internalError(buffer, 5);
    }
}

void VM::executeNativeMethod(const ClassInfo* targetClass, const MethodInfo* methodInfo, VMThread* thread)
{
    const std::string_view className = targetClass->getName();
    // printf("Running native code of method: %s.%s\n", className.data(), methodInfo->name.data());
    const std::string_view description = targetClass->constantPool->getString(methodInfo->descriptorIndex);
    const std::string_view methodName = methodInfo->name;
    std::string fullName = std::string{className};
    fullName += "/";
    fullName += methodName;
    nativeImplementation impl = findNativeMethod(fullName.c_str(), description.data());
    if (impl != nullptr)
    {
        NativeArgs nativeArgs{};
        nativeArgs.thread = thread;
        impl(nativeArgs);
    }
    else
    {
        char errorString[400];
        snprintf(errorString, 400, "Can't find native method %s %s", fullName.c_str(), description.data());
        thread->internalError(errorString, NATIVE_METHOD_NOT_FOUND);
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
    thread->m_currentFrame->localVariables[0] = Variable{VariableType_REFERENCE, arrayRef};

}

FieldInfo* VM::findField(ClassInfo* classInfo, const char* name, const char* descriptor, VMThread* thread)
{
    FieldInfo* targetField = classInfo->findField(name, descriptor);
    if (targetField == nullptr)
    {
        ClassInfo* currentClass = classInfo;

        while(currentClass != nullptr && currentClass->superClass != 0)
        {
            CPClassInfo* cpClassInfo = currentClass->constantPool->getClassInfo(currentClass->superClass);
            std::string_view superClassName =  currentClass->constantPool->getString(cpClassInfo->nameIndex);
            ClassInfo* superClass =  thread->getClass(superClassName);
            targetField = superClass->findField(name, descriptor);
            if (targetField != nullptr)
            {
                break;
            }
            currentClass = superClass;
        }

        if (targetField == nullptr)
        {
            thread->internalError("Static field not found");
        }
    }

    return targetField;
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

    printf("> Executing main method...\n");
    mainThread->executeLoop();
    // Object* object = heap.getObject(3);
    printf("> Done executing\n");
}

void VM::initSystemClass(ClassInfo* systemClass, VMThread* thread)
{
    MethodInfo* initMethod = systemClass->findMethodWithNameAndDescriptor("initializeSystemClass", "()V");

    thread->m_pc = 0;
    thread->m_currentClass = systemClass;
    thread->m_currentMethod = initMethod;

    thread->pushStackFrameSpecial(systemClass, initMethod, nullptr);

    printf("> Executing System init method...\n");
    thread->executeLoop();
    printf("> Done executing System init method\n");
}

void VM::shutdown()
{
    PHYSFS_deinit();
    Platform::cleanup();
}

void VM::checkType(const Variable var, const VariableType type, VMThread* thread)
{
    if (var.type != type)
    {
        thread->internalError("Error: Type mismatch", ErrorCode::TYPE_MISMATCH);
    }
}

VMThread* VM::getVMThreadByObjectRef(const u4 objectReference)
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

