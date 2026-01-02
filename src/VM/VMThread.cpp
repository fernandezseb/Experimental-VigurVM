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

#include "Error.h"
#include "VMThread.h"
#include "JavaHeap.h"
#include "VM.h"
#include "Library/Builtin.h"

#include <stack>

static constexpr std::string_view NoNonNativeStackFrameFound{"Can't return to previous frame because there is no previous non-native frame"};

ClassInfo* VMThread::getClass(std::string_view className)
{
    if (className.starts_with("["))
    {
        return nullptr;
    }
    ClassInfo* classInfo = VM::get()->getHeap()->getClassByName(className);
    if (classInfo == nullptr) {
        Memory *memory = new Memory(MIB(1), MIB(30));
        // printf("Loading class %s...\n", className.data());
        classInfo = VM::get()->getClassLoader().readClass(className.data(), memory, VM::get()->getConfiguration().m_classPath.data());
        initStaticFields(classInfo);
        VM::get()->getHeap()->addClassInfo(classInfo);
        runStaticInitializer(classInfo);
        return classInfo;
    }
    return classInfo;
}

void VMThread::executeNativeMethod(const ClassInfo* targetClass, const MethodInfo* methodInfo)
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
        nativeArgs.thread = this;
        impl(nativeArgs);
    }
    else
    {
        char errorString[400];
        snprintf(errorString, 400, "Can't find native method %s %s", fullName.c_str(), description.data());
        internalError(errorString, NATIVE_METHOD_NOT_FOUND);
    }
}

bool VMThread::isSubclass(const ClassInfo* targetClass, ClassInfo* subClass)
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
        currentClass = getClass(currentClass->constantPool->getString(classInfo->nameIndex));
    }

    return false;
}

FieldInfo* VMThread::findField(ClassInfo* classInfo, const char* name, const char* descriptor)
{
    FieldInfo* targetField = classInfo->findField(name, descriptor);
    if (targetField == nullptr)
    {
        ClassInfo* currentClass = classInfo;

        while(currentClass != nullptr && currentClass->superClass != 0)
        {
            CPClassInfo* cpClassInfo = currentClass->constantPool->getClassInfo(currentClass->superClass);
            std::string_view superClassName =  currentClass->constantPool->getString(cpClassInfo->nameIndex);
            ClassInfo* superClass =  getClass(superClassName);
            targetField = superClass->findField(name, descriptor);
            if (targetField != nullptr)
            {
                break;
            }
            currentClass = superClass;
        }

        if (targetField == nullptr)
        {
            internalError("Static field not found");
        }
    }

    return targetField;
}

void VMThread::initStaticFields(ClassInfo* class_info)
{
    u2 staticFieldsCount = 0;
    for (u2 currentField = 0; currentField < class_info->fieldsCount; ++currentField)
    {
        FieldInfo* field = class_info->fields[currentField];
        if (field->isStatic())
        {
            const VariableType type = fromDescriptor(class_info->constantPool->getString(class_info->fields[currentField]->descriptorIndex));
            const u1 varCount = getCategoryFromVariableType(type);
            staticFieldsCount += varCount;
        }
    }

    class_info->staticFieldsCount = staticFieldsCount;
    const auto variablesMemory = (vdata*) class_info->memory->alloc(sizeof(vdata) * staticFieldsCount);
    class_info->staticFields = std::span(variablesMemory, staticFieldsCount);

    u2 currentStaticField = 0;
    for (u2 currentField = 0; currentField < class_info->fieldsCount; ++currentField)
    {
        FieldInfo* field = class_info->fields[currentField];
        if (field->isStatic())
        {
            const VariableType type = fromDescriptor(class_info->constantPool->getString(field->descriptorIndex));
            const vdata variable(type);
            class_info->staticFields[currentStaticField++] = variable;
            if (variable.getCategory() == 2)
            {
                class_info->staticFields[currentStaticField++] = variable;
            }
            const std::size_t index = currentStaticField-variable.getCategory();
            field->staticData = class_info->staticFields[index];
        }
    }
}

void VMThread::runStaticInitializer(ClassInfo* classInfo)
{
    MethodInfo* entryPoint = classInfo->findMethodWithNameAndDescriptor("<clinit>", "()V");

    if (entryPoint == nullptr)
    {
        // No static initializers for this class
        return;
    }

    pushStackFrameWithoutParams(classInfo, entryPoint);

    // printf("Executing static initializers...\n");
    executeLoop();
}

void VMThread::pushStackFrameWithoutParams(ClassInfo* classInfo, const MethodInfo* methodInfo)
{
    StackFrame stackFrame;

    if (methodInfo->code == nullptr)
    {
        internalError(std::string_view("No code found in method"));
    }

    stackFrame.localVariables.reserve(methodInfo->code->maxLocals);
    for (u2  currentLocal = 0; currentLocal < methodInfo->code->maxLocals; ++currentLocal)
    {
        constexpr vdata var{VariableType_UNDEFINED};
        stackFrame.localVariables.push_back(var);
    }
    stackFrame.operands.reserve(methodInfo->code->maxStack);
    stackFrame.constantPool = classInfo->constantPool;
    stackFrame.previousPc = m_pc;
    stackFrame.previousClass = m_currentClass;
    stackFrame.previousMethod = m_currentMethod;
    stackFrame.className = classInfo->getName();
    stackFrame.methodName = methodInfo->name;


    m_pc = 0;
    m_currentClass = classInfo;
    m_currentMethod = methodInfo;

    this->m_stack.frames.push_back(stackFrame);
    m_currentFrame = &this->m_stack.frames[this->m_stack.frames.size()-1];
}

void VMThread::pushNativeStackFrame(ClassInfo* classInfo, const MethodInfo* methodInfo, size_t argumentsSize)
{
    StackFrame stackFrame;
    for (size_t currentLocal = 0; currentLocal < argumentsSize; ++currentLocal)
    {
        constexpr vdata var{VariableType_UNDEFINED};
        stackFrame.localVariables.push_back(var);
    }
    stackFrame.constantPool = classInfo->constantPool;
    stackFrame.previousPc = m_pc;
    stackFrame.previousClass = m_currentClass;
    stackFrame.previousMethod = m_currentMethod;
    stackFrame.className = classInfo->getName();
    stackFrame.methodName = methodInfo->name;
    stackFrame.isNative = true;


    m_pc = 0;
    m_currentClass = classInfo;
    m_currentMethod = methodInfo;

    this->m_stack.frames.push_back(stackFrame);
    m_currentFrame = &this->m_stack.frames[this->m_stack.frames.size()-1];
}

void VMThread::popFrame()
{
    const StackFrame* stackFrame = m_currentFrame;
    m_pc = stackFrame->previousPc;
    m_currentClass = stackFrame->previousClass;
    m_currentMethod = stackFrame->previousMethod;

    m_stack.frames.pop_back();
    if (!m_stack.frames.empty())
    {
        m_currentFrame = &m_stack.frames[m_stack.frames.size()-1];
    } else
    {
        m_currentFrame = nullptr;
    }
}


void VMThread::pushStackFrameSpecial(ClassInfo* classInfo, const MethodInfo* methodInfo, StackFrame* previousFrame)
{
    std::deque<vdata> arguments;
    if (previousFrame != nullptr)
    {
        // The arguments and the pointer to the object
        for (int i = methodInfo->argsCount; i >= 0; --i)
        {
            vdata operand = previousFrame->popOperand();
            if (operand.getCategory() == 2)
            {
                vdata highByte = previousFrame->popOperand();
                arguments.push_front(operand);
                arguments.push_front(highByte);
            } else {
                arguments.push_front(operand);
            }
        }

        const vdata ref = arguments[0];
        if (ref.type == VariableType_REFERENCE && ref.getReference() == 0)
        {
            internalError("NullpointerException in special call");
        }
    }

    if (methodInfo->isNative()) {
        pushNativeStackFrame(classInfo, methodInfo, arguments.size());
    } else {
        pushStackFrameWithoutParams(classInfo, methodInfo);
    }

    if (!arguments.empty())
    {
        for (int i = 0; i < arguments.size(); ++i)
        {
            m_currentFrame->localVariables[i] = arguments[i];
        }
    }
}

void VMThread::internalError(const std::string_view error, const i4 errorCode) const
{
    fprintf(stdout, "Unhandled VM error in thread \"%s\": %s\n", m_name.data(), error.data());
    for (i8 currentFrame = m_stack.frames.size() - 1; currentFrame >= 0; --currentFrame)
    {
        const StackFrame frame = m_stack.frames[currentFrame];
        const char *nativeText = "";
        if (frame.isNative)
        {
            nativeText = " (native)";
        }
        printf("    at %s.%s%s\n", frame.className.data(), frame.methodName.data(), nativeText);
    }
    Platform::exitProgram(errorCode);
}

void VMThread::internalError(std::string_view error) const
{
    fprintf(stdout, "Unhandled VM error in thread \"%s\": %s\n", m_name.data(), error.data());
    for (i8 currentFrame = m_stack.frames.size() - 1; currentFrame >= 0; --currentFrame)
    {
        const StackFrame frame = m_stack.frames[currentFrame];
        const char *nativeText = "";
        if (frame.isNative)
        {
            nativeText = " (native)";
        }
        printf("    at %s.%s%s\n", frame.className.data(), frame.methodName.data(), nativeText);
    }
    Platform::exitProgram(-79);
}

u1 VMThread::readUnsignedByte()
{
    if (m_pc >= m_currentMethod->code->codeLength)
    {
        printf("Trying to access code at location: %d, whereas code has length: %d\n", m_pc, m_currentMethod->code->codeLength);
        internalError("Trying to access code at invalid location", 5);
    }
    return m_currentMethod->code->code[m_pc++];
}

u2 VMThread::readUnsignedShort()
{
    const u1* code = m_currentMethod->code->code;
    const uint8_t indexByte1 = code[m_pc++];
    const uint8_t indexByte2 = code[m_pc++];
    const uint16_t shortCombined = (indexByte1 << 8) | indexByte2;
    return shortCombined;
}

i2 VMThread::readSignedShort()
{
    const u1* code = m_currentMethod->code->code;
    const u1 buffer[2] = {code[m_pc++], code[m_pc++]};
    const i2 value = static_cast<i2>(buffer[1]) | static_cast<i2>(buffer[0]) << 8;
    return value;
}

i4 VMThread::readSignedInt()
{
    const u1* code = m_currentMethod->code->code;
    const uint8_t buffer[4] = {code[m_pc++], code[m_pc++], code[m_pc++], code[m_pc++]};
    const int32_t value = static_cast<int32_t>(buffer[3])
                          | static_cast<int32_t>(buffer[2]) << 8
                          | static_cast<int32_t>(buffer[1]) << 16
                          | static_cast<int32_t>(buffer[0]) << 24;
    return value;
}

StackFrame* VMThread::getTopFrameNonNative()
{
    for (i8 currentFrame = m_stack.frames.size()-1; currentFrame>=0; --currentFrame)
    {
        StackFrame* frame = &m_stack.frames[currentFrame];
        if (!frame->isNative)
        {
            return frame;
        }
    }

    return nullptr;
}

void VMThread::executeLoop()
{
    const std::size_t stackSize = m_stack.frames.size();
    const std::size_t depth = stackSize == 0 ? 0 : stackSize-1;
    uint8_t args[10];
    while (m_stack.frames.size() > depth)
    {
        uint8_t opcode = readUnsignedByte();
        // printf("Running instruction with opcode: 0x%0x \n", opcode);

        bool found = false;

        for (const Instruction& instruction : VM::getInstructions()) {
            if (((u1)instruction.opcode) == opcode)
            {
                found = true;
                // printf("%s\n", instruction.name.data());
                if (instruction.args > 0) {
                    for (u2 currentArg = 0; currentArg < instruction.args; ++currentArg)
                    {
                        args[currentArg] = readUnsignedByte();
                    }
                }
                if (instruction.instructionFunction != nullptr) {
                    InstructionInput input = {};
                    input.args = args;
                    input.argsCount = instruction.args;
                    input.arg = instruction.arg;
                    input.thread = this;
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
            internalError(buffer, 78);
        }
    }
}

void VMThread::returnVar(const vdata returnValue)
{
    StackFrame* targetFrame = getTopFrameNonNative();
    if (targetFrame != nullptr)
    {
        targetFrame->operands.push_back(returnValue);
        if (returnValue.getCategory() == 2) // TODO: Check if 1 data item is enough
        {
            targetFrame->operands.push_back(returnValue);
        }
    }
    else
    {
        internalError(NoNonNativeStackFrameFound);
    }
}

void VMThread::pushStackFrameStatic(ClassInfo* classInfo, MethodInfo* methodInfo, StackFrame* previousFrame)
{
    pushStackFrameWithoutParams(classInfo, methodInfo);

    if (previousFrame != nullptr)
    {
        std::deque<vdata> arguments;
        // The arguments
        for (int i = methodInfo->argsCount; i > 0; --i)
        {
            vdata operand = previousFrame->popOperand();
            if (operand.getCategory() == 2)
            {
                vdata highByte = previousFrame->popOperand();
                arguments.push_front(operand);
                arguments.push_front(highByte);
            } else {
                arguments.push_front(operand);
            }
        }

        if (!arguments.empty())
        {
            for (int i = 0; i < arguments.size(); ++i)
            {
                m_currentFrame->localVariables[i] = arguments[i];
            }
        }

    }
}
