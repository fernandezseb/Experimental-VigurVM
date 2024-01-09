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

void VMThread::pushStackFrameWithoutParams(ClassInfo* classInfo, const MethodInfo* methodInfo)
{
    StackFrame stackFrame;
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

    this->m_stackstack.top().frames.push_back(stackFrame);
    m_currentFrame = &this->m_stackstack.top().frames[this->m_stackstack.top().frames.size()-1];
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

    this->m_stackstack.top().frames.push_back(stackFrame);
    m_currentFrame = &this->m_stackstack.top().frames[this->m_stackstack.top().frames.size()-1];
}

void VMThread::popFrame()
{
    const StackFrame* stackFrame = m_currentFrame;
    m_pc = stackFrame->previousPc;
    m_currentClass = stackFrame->previousClass;
    m_currentMethod = stackFrame->previousMethod;

    m_stackstack.top().frames.pop_back();
    if (!m_stackstack.top().frames.empty())
    {
        m_currentFrame = &m_stackstack.top().frames[m_stackstack.top().frames.size()-1];
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
                arguments.push_front(highByte);
            }
            arguments.push_front(operand);
        }

        const Variable ref = arguments[0];
        if (ref.type == VariableType_REFERENCE && ref.data == 0)
        {
            internalError("NullpointerException in special call");
        }
    }

    pushStackFrameWithoutParams(classInfo, methodInfo);
    if (!arguments.empty())
    {
        for (int i = 0; i < arguments.size(); ++i)
        {
            m_currentFrame->localVariables[i] = arguments[i];
        }
    }
}

void VMThread::internalError(const char* error)
{
    fprintf(stdout, "Unhandled VM error in thread \"%s\": %s\n", m_name.data(), error);
    while (!m_stackstack.empty()) {
        if (!m_stackstack.top().frames.empty())
        {
            for (i8 currentFrame = m_stackstack.top().frames.size() - 1; currentFrame >= 0; --currentFrame)
            {
                const StackFrame frame = m_stackstack.top().frames[currentFrame];
                const char *nativeText = "";
                if (frame.isNative)
                {
                    nativeText = " (native)";
                }
                printf("    at %s.%s%s\n", frame.className, frame.methodName, nativeText);
            }
        }
        m_stackstack.pop();
    }
    Platform::exitProgram(6);
}

void VMThread::returnVar(const Variable returnValue)
{
    JavaStack* topStack = &m_stackstack.top();
    if (topStack->frames.size() > 1)
    {
        StackFrame* previousStackFrame = &topStack->frames[topStack->frames.size()-2];
        previousStackFrame->operands.push_back(returnValue);
    } else
    {
        internalError("Can't return to previous frame because there is no previous frame");
    }
}

void VMThread::returnVar(Variable highByte, Variable lowByte)
{
    JavaStack* topStack = &m_stackstack.top();
    if (topStack->frames.size() > 1)
    {
        StackFrame* previousStackFrame = &topStack->frames[topStack->frames.size()-2];
        previousStackFrame->operands.push_back(highByte);
        previousStackFrame->operands.push_back(lowByte);
    } else
    {
        internalError("Can't return to previous frame because there is no previous frame");
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
