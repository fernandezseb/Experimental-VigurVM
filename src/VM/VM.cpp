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

#include "VM.h"

#include "Configuration.h"
#include "Memory.h"
#include "Library/Builtin.h"

#include <stack>
#include <string>
#include <variant>

VM::VM(const Configuration configuration) noexcept
    : m_bootstrapClassLoader(), m_configuration(configuration)
{
}

void VM::start()
{
    Platform::initialize();

    registerBuiltinRegisterNatives();
    getClass("java/lang/OutOfMemoryError", &m_mainThread);
    getClass("java/lang/VirtualMachineError", &m_mainThread);
    getClass("java/lang/Object", &m_mainThread);
    getClass("java/lang/Number", &m_mainThread);
    getClass("java/lang/Class", &m_mainThread);
    getClass("java/lang/String", &m_mainThread);
    getClass("java/lang/System", &m_mainThread);
    getClass("java/lang/Thread", &m_mainThread);
    getClass("java/lang/ThreadGroup", &m_mainThread);

    const u4 threadGroupReference = createThreadGroupObject(&m_mainThread);
    m_mainThread.threadObject = createThreadObject(&m_mainThread, threadGroupReference);
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

std::vector<Variable> VM::createVariableForDescriptor(const char* descriptor)
{
    std::vector<Variable> variables;
    if (strcmp(descriptor, "I") == 0)
    {
        constexpr Variable variable{VariableType_INT};
        variables.push_back(variable);
    } else if (strcmp(descriptor, "Z") == 0)
    {
        constexpr Variable variable{VariableType_INT};
        variables.push_back(variable);
    } else if (strcmp(descriptor, "B") == 0)
    {
        constexpr Variable variable{VariableType_INT};
        variables.push_back(variable);
    } else if (strcmp(descriptor, "J") == 0)
    {
        constexpr Variable variableLow{VariableType_LONG};
        constexpr Variable variableHigh{VariableType_LONG};
        variables.push_back(variableHigh);
        variables.push_back(variableLow);
    } else if (strcmp(descriptor, "D") == 0)
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
    }
    else
    {
        fprintf(stderr, "Error: Couldn't construct data for descriptor type: %s\n", descriptor);
        Platform::exitProgram(7);
    }
    return variables;
}

u1 VM::getDescriptorVarCategory(const char* descriptor) noexcept
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
    // TODO: Do it for superclasses as well?
    u2 staticFieldsCount = 0;
    for (u2 currentField = 0; currentField < class_info->fieldsCount; ++currentField)
    {
        FieldInfo* field = class_info->fields[currentField];
        if (field->isStatic())
        {
            const u1 varCount = getDescriptorVarCategory(class_info->constantPool->getString(class_info->fields[currentField]->descriptorIndex).data());
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
            std::vector<Variable> variables = createVariableForDescriptor(class_info->constantPool->getString(field->descriptorIndex).data());
            for (Variable variable : variables) {
                class_info->staticFields[currentStaticField++] = variable;
            }
            const std::size_t index = currentStaticField-variables.size();
            field->staticData = &class_info->staticFields[index];
        }
    }
}

void VM::updateVariableFromVariable(Variable* variable, const char* descriptor, Variable operand, Variable operand2, VMThread* thread)
{
    if (strcmp(descriptor, "I") == 0)
    {
        checkType(*variable, VariableType_INT, thread);
        checkType(operand, VariableType_INT, thread);

        variable->data = operand.data;
    } else if (strcmp(descriptor, "Z") == 0)
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
        snprintf(buffer, 200, "Error: Setting of variable of type with descriptor: %s not implemented yet!\n", descriptor);
        thread->internalError(buffer);
    }
}

static u1 readByte(VMThread* thread)
{
    return thread->m_currentMethod->code->code[thread->m_pc++];
}

[[maybe_unused]]
static u2 readShort(VMThread* thread)
{
    uint8_t indexByte1 = thread->m_currentMethod->code->code[thread->m_pc++];
    uint8_t indexByte2 = thread->m_currentMethod->code->code[thread->m_pc++];
    uint16_t shortCombined = (indexByte1 << 8) | indexByte2;
    return shortCombined;
}

void VM::executeLoop(VMThread* thread)
{
    const std::size_t stackSize = thread->m_stack.frames.size();
    const std::size_t depth = stackSize == 0 ? 0 : stackSize-1;
    while(thread->m_stack.frames.size() > depth)
    {
        uint8_t opcode = readByte(thread);
        printf("Running instruction with opcode: 0x%0x ", opcode);

        bool found = false;

        for (const Instruction& instruction : m_instructions) {
            if (((u1)instruction.opcode) == opcode)
            {
                found = true;
                printf("%s\n", instruction.name);
                uint8_t* args = 0;
                if (instruction.args > 0) {
                    args = (uint8_t*)Platform::allocateMemory(instruction.args, 0);
                    for (u2 currentArg = 0; currentArg < instruction.args; ++currentArg)
                    {
                        args[currentArg] = readByte(thread);
                    }
                }
                if (instruction.instructionFunction != NULL) {
                    instruction.instructionFunction(args, instruction.args, instruction.arg, &m_heap, thread, this);
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

ClassInfo* VM::getClass(const char* className, VMThread* thread)
{
    ClassInfo* classInfo = m_heap.getClassByName(className);
    if (classInfo == nullptr) {
        Memory *memory = new Memory(MIB(1), MIB(30));
        printf("Loading class %s...\n", className);
        classInfo = m_bootstrapClassLoader.readClass(className, memory, m_configuration.classPath.data());
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
    printf("Running native code of method: %s\n", className.data());
    const std::string_view description = targetClass->constantPool->getString(methodInfo->descriptorIndex);
    const std::string_view methodName = methodInfo->name;
    std::string fullName = std::string{className};
    fullName += "/";
    fullName += methodName;
    nativeImplementation impl = findNativeMethod(fullName.c_str(), description.data());
    if (impl != nullptr)
    {
        impl(heap, thread, this);
    }
    else
    {
        char errorString[400];
        snprintf(errorString, 400, "Can't find native method %s %s", fullName.c_str(), description.data());
        thread->internalError(errorString);
    }
}

void VM::runMain()
{
    VMThread* mainThread = &m_mainThread;
    if (m_configuration.mainClassName.empty())
    {
        fprintf(stderr, "Error: Class name of starting class not defined..\n");
        Platform::exitProgram(6);
    }

    Memory memory(1000, KIB(5));
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

    // TODO: Put string array in local variable with index 0

    printf("> Executing main method...\n");
    executeLoop(mainThread);
    // Object* object = heap.getObject(3);
    printf("> Done executing\n");
}

void VM::shutdown()
{
    Platform::cleanup();
}

void VM::checkType(const Variable var, const VariableType type, VMThread* thread)
{
    if (var.type != type)
    {
        thread->internalError("Error: TypeMismatch");
    }
}
