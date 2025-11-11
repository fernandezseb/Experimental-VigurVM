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

#include "Configuration.h"
#include "Memory.h"
#include "Library/Builtin.h"
#include "physfs.h"

#include <stack>
#include <string>
#include <variant>

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

    registerBuiltinRegisterNatives();
    getClass("java/lang/OutOfMemoryError", &m_mainThread);
    getClass("java/lang/VirtualMachineError", &m_mainThread);
    getClass("java/lang/Object", &m_mainThread);
    getClass("java/lang/Number", &m_mainThread);
    ClassInfo* classInfo = getClass("java/lang/Class", &m_mainThread);
    getClass("java/lang/String", &m_mainThread);
    ClassInfo* systemClass = getClass("java/lang/System", &m_mainThread);
    getClass("java/lang/Thread", &m_mainThread);
    getClass("java/lang/ThreadGroup", &m_mainThread);
    getClass("java/lang/reflect/Field", &m_mainThread);
    getClass("java/lang/reflect/AccessibleObject", &m_mainThread);
    getClass("java/lang/reflect/Executable", &m_mainThread);
    getClass("java/lang/reflect/Constructor", &m_mainThread);
    getClass("java/io/PrintStream", &m_mainThread);
    getClass("java/io/FilterOutputStream", &m_mainThread);
    getClass("java/io/OutputStream", &m_mainThread);
    getClass("java/util/Properties", &m_mainThread);

    m_heap.setClassInfo(classInfo);

    const u4 threadGroupReference = createThreadGroupObject(&m_mainThread);
    m_mainThread.threadObject = createThreadObject(&m_mainThread, threadGroupReference);

    // TODO: Enable when fixed
    // initSystemClass(systemClass, &m_mainThread);
}

u4 VM::createThreadGroupObject(VMThread* thread)
{
    ClassInfo* threadGroupClass = getClass("java/lang/ThreadGroup", thread);
    const u4 threadGroupReference = m_heap.createObject(threadGroupClass, this);
    const Object* threadGroupObject = m_heap.getObject(threadGroupReference);

    const FieldData* maxPrioField = threadGroupObject->getField("maxPriority", "I", &m_heap);
    maxPrioField->data->data = 10;


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
        currentClass = getClass(currentClass->constantPool->getString(classInfo->nameIndex), thread);
    }

    return false;
}

u4 VM::createThreadObject(VMThread* thread, const u4 threadGroupReference)
{
    ClassInfo* threadClass = getClass("java/lang/Thread", thread);
    const u4 objectReference = m_heap.createObject(threadClass, this);
    const Object* threadObject = m_heap.getObject(objectReference);

    const FieldData* groupField = threadObject->getField("group", "Ljava/lang/ThreadGroup;", &m_heap);
    groupField->data->data = threadGroupReference;

    const FieldData* priorityField = threadObject->getField("priority", "I", &m_heap);
    priorityField->data->data = thread->priority;

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
        constexpr Variable variable{VariableType_CHAR};
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

void VM::initStaticFields(ClassInfo* class_info, [[maybe_unused]] VMThread* thread)
{
    u2 staticFieldsCount = 0;
    for (u2 currentField = 0; currentField < class_info->fieldsCount; ++currentField)
    {
        FieldInfo* field = class_info->fields[currentField];
        if (field->isStatic())
        {
            const u1 varCount = getDescriptorVarCategory(class_info->constantPool->getString(class_info->fields[currentField]->descriptorIndex));
            staticFieldsCount += varCount;
        }
    }

    class_info->staticFieldsCount = staticFieldsCount;
    const auto variablesMemory = (Variable*) class_info->memory->alloc(sizeof(Variable) * staticFieldsCount);
    class_info->staticFields = std::span(variablesMemory, staticFieldsCount);

    u2 currentStaticField = 0;
    for (u2 currentField = 0; currentField < class_info->fieldsCount; ++currentField)
    {
        FieldInfo* field = class_info->fields[currentField];
        if (field->isStatic())
        {
            std::vector<Variable> variables = createVariableForDescriptor(class_info->constantPool->getString(field->descriptorIndex));
            for (Variable variable : variables) {
                class_info->staticFields[currentStaticField++] = variable;
            }
            const std::size_t index = currentStaticField-variables.size();
            field->staticData = &class_info->staticFields[index];
        }
    }
}

void VM::updateVariableFromVariable(Variable* variable, std::string_view descriptor, Variable operand, Variable operand2, VMThread* thread)
{
    if (descriptor ==  "I")
    {
        checkType(*variable, VariableType_INT, thread);
        checkType(operand, VariableType_INT, thread);

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
        thread->internalError(buffer);
    }
}

void VM::executeLoop(VMThread* thread)
{
    const std::size_t stackSize = thread->m_stack.frames.size();
    const std::size_t depth = stackSize == 0 ? 0 : stackSize-1;
    while (thread->m_stack.frames.size() > depth)
    {
        uint8_t opcode = thread->readUnsignedByte();
        // printf("Running instruction with opcode: 0x%0x \n", opcode);

        bool found = false;

        for (const Instruction& instruction : m_instructions) {
            if (((u1)instruction.opcode) == opcode)
            {
                found = true;
                // printf("%s\n", instruction.name.data());
                uint8_t* args = 0;
                if (instruction.args > 0) {
                    args = (uint8_t*)Platform::allocateMemory(instruction.args, 0);
                    for (u2 currentArg = 0; currentArg < instruction.args; ++currentArg)
                    {
                        args[currentArg] = thread->readUnsignedByte();
                    }
                }
                if (instruction.instructionFunction != nullptr) {
                    InstructionInput input = {};
                    input.args = args;
                    input.argsCount = instruction.args;
                    input.arg = instruction.arg;
                    input.heap = &m_heap;
                    input.thread = thread;
                    input.vm = this;
                    instruction.instructionFunction(input);
                }
                break;
            }
        }

        if (found)[[likely]]
        {
            continue;
        } else [[unlikely]] {
            printf("\n");
            char buffer[200];
            snprintf(buffer, 200, "Unrecognized opcode detected: 0x%0x", opcode);
            thread->internalError(buffer);
        }
    }
}

void VM::runStaticInitializer(ClassInfo* classInfo, VMThread* thread)
{
    MethodInfo* entryPoint = classInfo->findMethodWithNameAndDescriptor("<clinit>", "()V");

    if (entryPoint == nullptr)
    {
        // No static initializers for this class
        return;
    }

    thread->pushStackFrameWithoutParams(classInfo, entryPoint);

    printf("Executing static initializers...\n");
    executeLoop(thread);
}

ClassInfo* VM::getClass(std::string_view className, VMThread* thread)
{
    if (className.starts_with("["))
    {
        return nullptr;
    }
    ClassInfo* classInfo = m_heap.getClassByName(className);
    if (classInfo == nullptr) {
        Memory *memory = new Memory(MIB(1), MIB(30));
        printf("Loading class %s...\n", className.data());
        classInfo = m_bootstrapClassLoader.readClass(className.data(), memory, m_configuration.classPath.data());
        initStaticFields(classInfo, thread);
        m_heap.addClassInfo(classInfo);
        runStaticInitializer(classInfo, thread);
        return classInfo;
    }
    return classInfo;
}

void VM::executeNativeMethod(const ClassInfo* targetClass, const MethodInfo* methodInfo, JavaHeap* heap, VMThread* thread)
{
    const std::string_view className = targetClass->getName();
    printf("Running native code of method: %s.%s\n", className.data(), methodInfo->name.data());
    const std::string_view description = targetClass->constantPool->getString(methodInfo->descriptorIndex);
    const std::string_view methodName = methodInfo->name;
    std::string fullName = std::string{className};
    fullName += "/";
    fullName += methodName;
    nativeImplementation impl = findNativeMethod(fullName.c_str(), description.data());
    if (impl != nullptr)
    {
        NativeArgs nativeArgs{};
        nativeArgs.heap = heap;
        nativeArgs.thread = thread;
        nativeArgs.vm = this;
        impl(nativeArgs);
    }
    else
    {
        char errorString[400];
        snprintf(errorString, 400, "Can't find native method %s %s", fullName.c_str(), description.data());
        thread->internalError(errorString);
    }
}

void VM::createArgsArray(const VMThread* thread)
{
    const u4 arrayRef =  m_heap.createArray(AT_REFERENCE, m_configuration.args.size(), "Ljava/lang/String;");
    const Array* array = m_heap.getArray(arrayRef);
    u4* arrayData = reinterpret_cast<u4*>(array->data);
    u4 currentArg = 0;
    for (std::string_view arg : m_configuration.args)
    {
        const u4 stringRef = m_heap.createString(arg.data(), this);
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
            ClassInfo* superClass =  getClass(superClassName, thread);
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
    VMThread* mainThread = &m_mainThread;
    if (m_configuration.mainClassName.empty())
    {
        fprintf(stderr, "Error: Class name of starting class not defined..\n");
        Platform::exitProgram(6);
    }

    ClassInfo* startupClass = getClass(m_configuration.mainClassName.data(), mainThread);
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
    executeLoop(mainThread);
    // Object* object = heap.getObject(3);
    printf("> Done executing\n");
}

void VM::initSystemClass(ClassInfo* systemClass, VMThread* thread)
{
    MethodInfo* initMethod = systemClass->findMethodWithNameAndDescriptor("initializeSystemClass", "()V");

    thread->m_pc = 0;
    thread->m_currentClass = systemClass;
    thread->m_currentMethod = initMethod;

    thread->pushStackFrameSpecial(systemClass, initMethod, nullptr, &m_heap);

    printf("> Executing System init method...\n");
    executeLoop(thread);
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
        thread->internalError("Error: TypeMismatch");
    }
}

