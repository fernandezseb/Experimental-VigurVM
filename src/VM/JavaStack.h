#pragma once

#include <bit>
#include <vector>

#include "Core.h"
#include "Data/Class.h"
#include "Data/Variable.h"

class ConstantPool;

struct StackFrame {
    // Local variable array
    std::vector<vdata> localVariables;
    // operand stack, where long and double are two values
    std::vector<vdata> operands;

    // reference to runtime constant pool, for the type of the current method
    ConstantPool* constantPool{};

    // used to restore the state of the invoker,
    // pc of the invoker
    u4 previousPc{};

    ClassInfo* previousClass{};
    const MethodInfo* previousMethod{};

    std::string_view className{};
    std::string_view methodName{};
    bool isNative = false;

    vdata popOperand()
    {
        if (operands.empty())
        {
            fprintf(stderr, "Error: No operands on stack found!\n");
            Platform::exitProgram(78);
        }
        const vdata var = operands.back();
        operands.pop_back();
        return var;
    }

    i8 popLong()
    {
        vdata first = popOperand();
        vdata second = popOperand();
        return first.getLong();
    }

    void pushLong(vlong value)
    {
        // TODO: Check if we can just push 1 long instead of 2 and still be compatible
        // You can't because the vars are indexed
        operands.emplace_back(VariableType_LONG, value);
        operands.emplace_back(VariableType_LONG, value);
    }

    i4 popInt()
    {
        const vdata var = popOperand();
        return var.getInt();
    }

    void pushInt(const vint value)
    {
        operands.emplace_back(VariableType_INT, value);
    }

    float popFloat()
    {
        const vdata var = popOperand();
        return var.getFloat();
    }

    void pushFloat(const vfloat value)
    {
        operands.emplace_back(VariableType_FLOAT, value);
    }

    void pushObject(const u4 objectRef)
    {
        operands.emplace_back(VariableType_REFERENCE, objectRef);
    }

    [[nodiscard]] vdata peekOperand() const
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