#pragma once

#include <bit>
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
    const MethodInfo* previousMethod{};

    const char* className{};
    const char* methodName{};
    bool isNative = false;

    Variable popOperand()
    {
        if (operands.empty())
        {
            fprintf(stderr, "Error: No operands on stack found!\n");
            Platform::exitProgram(78);
        }
        const Variable var = operands.back();
        operands.pop_back();
        return var;
    }

    i8 popLong()
    {
        const Variable varlowByte = popOperand();
        const Variable varHighByte = popOperand();

        const u8 u1 = ((static_cast<u8>(varHighByte.data) << 32) | static_cast<u8>(varlowByte.data));
        return std::bit_cast<i8>(u1);
    }

    void pushLong(i8 value)
    {
        const auto parts = reinterpret_cast<u4*>(&value);
        operands.emplace_back(VariableType_LONG, parts[1]);
        operands.emplace_back(VariableType_LONG, parts[0]);
    }

    i4 popInt()
    {
        const Variable var = popOperand();
        return std::bit_cast<i4>(var.data);
    }

    void pushInt(const i4 value)
    {
        operands.emplace_back(VariableType_INT, std::bit_cast<u4>(value));
    }

    float popFloat()
    {
        const Variable var = popOperand();
        return std::bit_cast<float>(var.data);
    }

    void pushFloat(const float value)
    {
        operands.emplace_back(VariableType_FLOAT, std::bit_cast<u4>(value));
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