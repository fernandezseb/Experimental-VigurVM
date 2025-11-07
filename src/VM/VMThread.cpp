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

#include "VMThread.h"

#include "JavaHeap.h"

#include <stack>

static constexpr std::string_view NoNonNativeStackFrameFound{"Can't return to previous frame because there is no previous non-native frame"};

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
        constexpr Variable var{VariableType_UNDEFINED};
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
        constexpr Variable var{VariableType_UNDEFINED};
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


void VMThread::pushStackFrameSpecial(ClassInfo* classInfo, const MethodInfo* methodInfo, StackFrame* previousFrame,
    [[maybe_unused]] JavaHeap* heap)
{
    std::deque<Variable> arguments;
    if (previousFrame != nullptr)
    {
        // The arguments and the pointer to the object
        for (int i = methodInfo->argsCount; i >= 0; --i)
        {
            Variable operand = previousFrame->popOperand();
            if (operand.getCategory() == 2)
            {
                Variable highByte = previousFrame->popOperand();
                arguments.push_front(operand);
                arguments.push_front(highByte);
            } else {
                arguments.push_front(operand);
            }
        }

        const Variable ref = arguments[0];
        if (ref.type == VariableType_REFERENCE && ref.data == 0)
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

void VMThread::internalError(const std::string_view error) const
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
    Platform::exitProgram(6);
}

u1 VMThread::readUnsignedByte()
{
    if (m_pc >= m_currentMethod->code->codeLength)
    {
        printf("Trying to access code at location: %d, whereas code has length: %d\n", m_pc, m_currentMethod->code->codeLength);
        internalError("Trying to access code at invalid location");
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

void VMThread::returnVar(const Variable returnValue)
{
    StackFrame* targetFrame = getTopFrameNonNative();
    if (targetFrame != nullptr)
    {
        targetFrame->operands.push_back(returnValue);
    }
    else
    {
        internalError(NoNonNativeStackFrameFound);
    }
}

void VMThread::returnVar(Variable highByte, Variable lowByte)
{
    StackFrame* targetFrame = getTopFrameNonNative();
    if (targetFrame != nullptr)
    {
        targetFrame->operands.push_back(highByte);
        targetFrame->operands.push_back(lowByte);
    } else
    {
        internalError(NoNonNativeStackFrameFound);
    }
}

void VMThread::pushStackFrameStatic(ClassInfo* classInfo, MethodInfo* methodInfo, StackFrame* previousFrame)
{
    pushStackFrameWithoutParams(classInfo, methodInfo);

    if (previousFrame != nullptr)
    {
        // The arguments
        for (int i = methodInfo->argsCount; i > 0; --i)
        {
            m_currentFrame->localVariables[i-1] = previousFrame->popOperand();
        }
    }
}
