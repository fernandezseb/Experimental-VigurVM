#pragma once

#include <vector>

#include "Core.h"
#include "Data/Class.h"
#include "Data/Variable.h"

class ConstantPool;

struct StackFrame {
    // Local variable array
    std::vector<Variable> localVariables;
    // operand stack, where long and double are two values
    std::vector<Variable> operands;

    // reference to runtime constant pool, for the type of the current method
    ConstantPool* constantPool{};

    // used to restore the state of the invoker,
    // pc of the invoker
    u4 previousPc{};

    ClassInfo* previousClass{};
    MethodInfo* previousMethod{};

    const char* className{};
    const char* methodName{};

    Variable popOperand()
    {
        if (operands.empty())
        {
            fprintf(stderr, "Error: No operands on stack found!\n");
            Platform::exitProgram(-78);
        }
        Variable var = operands.back();
        operands.pop_back();
        return var;
    }

    [[nodiscard]] Variable peekOperand() const
    {
        return operands.back();
    }
};

class JavaStack {
public:
    std::vector<StackFrame> frames;
    explicit JavaStack(const size_t frameSize)
    {
        frames.reserve(frameSize);
    }
};