#pragma once

#include "Core.h"
#include "Configuration.h"
#include "ClassLoader/ClassLoader.h"
#include "Data/Variable.h"
#include "JavaHeap.h"

#include <vector>

struct StackFrame {
    // Local variable array
    std::vector<Variable> localVariables;
    // operand stack, where long and double are two values
    std::vector<Variable> operands;

    // reference to runtime constant pool, for the type of the current method
    ConstantPool* constantPool;

    // used to restore the state of the invoker,
    // pc of the invoker
    u4 previousPc;

    ClassInfo* previousClass;
    MethodInfo* previousMethod;

    Variable popOperand()
    {
        Variable var = operands.back();
        operands.pop_back();
        return var;
    }

    Variable peekOperand()
    {
        return operands.back();
    }
};

class JavaStack {
public:
    std::vector<StackFrame> frames;
};

class VMThread {
public:
    u4 pc;
    JavaStack stack;
    // Current frame
    StackFrame* currentFrame;
    // Current method
    MethodInfo* currentMethod;
    // current class
    ClassInfo* currentClass;
    char* name;
};

class VM {
private:
    ClassLoader bootstrapClassLoader;
    JavaHeap heap;
    VMThread thread;
    Configuration configuration;
    static std::vector<Variable> createVariableForDescriptor(char* descriptor);
    static uint16_t getDescriptorVarCount(char* get_string);
    void initStaticFields(ClassInfo* class_info);
    void updateVariableFromOperand(Variable* variable, char* get_string, StackFrame* stack_frame);
    void executeLoop();
    void pushStackFrameStatic(ClassInfo* classInfo, MethodInfo* methodInfo, StackFrame* previousFrame);
    void pushStackFrameWithoutParams(ClassInfo* classInfo, MethodInfo* methodInfo);
    void pushStackFrameVirtual(ClassInfo* classInfo, MethodInfo* methodInfo, StackFrame* previousFrame);
    void runStaticInitializer(ClassInfo* class_info);
public:
    VM();
    void start(Configuration configuration);
    ClassInfo* getClass(const char* className);
    void runMain(const char* className);
};
