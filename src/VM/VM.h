#pragma once

#include "Core.h"
#include "Configuration.h"
#include "ClassLoader/ClassLoader.h"
#include "Data/Variable.h"
#include "Data/Opcode.h"
#include "JavaHeap.h"

#include <vector>

#include "ConstantInstructions.h"
#include "LoadInstructions.h"
#include "ReferenceInstructions.h"
#include "StoreInstructions.h"
#include "StackInstructions.h"

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
        if (operands.empty())
        {
            fprintf(stderr, "Error: No operands on stack found!\n");
            Platform::exitProgram(-78);
        }
        Variable var = operands.back();
        operands.pop_back();
        return var;
    }

    Variable peekOperand() const
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

struct VM;

struct Instruction
{
    Opcode opcode;
    u1 args;
    const char* name;
    i1 arg;
    void(*instructionFunction)(u1* args, u2 argsCount, i1 arg, JavaHeap* heap, VMThread* thread, VM* VM);
};


class VM {
private:
    Instruction instructions[33] =
    {
        // Constants
        {i_nop, 0, "nop", 0, nop},
        {i_aconst_null, 0, "aconst_null", 0, aconst_null},
        {i_iconst_m1, 0, "iconst_m1", -1, iconst_i},
        {i_iconst_0, 0, "iconst_0", 0, iconst_i},
        {i_iconst_1, 0, "iconst_1", 1, iconst_i},
        {i_iconst_2, 0, "iconst_2", 2, iconst_i},
        {i_iconst_3, 0, "iconst_3", 3, iconst_i},
        {i_iconst_4, 0, "iconst_4", 4, iconst_i},
        {i_iconst_5, 0, "iconst_5", 5, iconst_i},
        {i_lconst_0, 0, "lconst_0", 0, lconst_i},
        {i_lconst_1, 0, "lconst_1", 1, lconst_i},
        {i_fconst_0, 0, "fconst_0", 0, fconst_i},
        {i_fconst_1, 0, "fconst_1", 1, fconst_i},
        {i_fconst_2, 0, "fconst_2", 2, fconst_i},
        {i_dconst_0, 0, "dconst_0", 0, dconst_i},
        {i_dconst_1, 0, "dconst_1", 1, dconst_i},
        {i_bipush, 1, "bipush", 0, bipush},
        {i_sipush, 2, "sipush", 0, sipush},
        {i_ldc, 1, "ldc", 0, ldc},
        {i_ldc_w, 2, "ldc_w", 0, ldc_w},
        {i_ldc2_w, 2, "ldc2_w", 0, ldc2_w},
        // Loads
        {i_aload_0, 0, "aload_0", 0, aload_i},
        {i_aload_1, 0, "aload_1", 1, aload_i},
        {i_aload_2, 0, "aload_2", 2, aload_i},
        {i_aload_3, 0, "aload_3", 3, aload_i},
        // Stores
        {i_astore_0, 0, "astore_0", 0, astore_i},
        {i_astore_1, 0, "astore_1", 1, astore_i},
        {i_astore_2, 0, "astore_2", 2, astore_i},
        {i_astore_3, 0, "astore_3", 3, astore_i},
        // Stack
        {i_dup, 0, "dup", 0, dup},
        // Reference
        {i_getstatic, 0, "getstatic", 0, getstatic},
        {i_putstatic, 0, "putstatic", 0, putstatic},
        {i_putfield, 0, "putfield", 0, putfield},

    };
    ClassLoader bootstrapClassLoader;
    JavaHeap heap;
    VMThread thread;
    Configuration configuration;
    static std::vector<Variable> createVariableForDescriptor(char* descriptor);
    void initStaticFields(ClassInfo* class_info);
    void executeLoop(VMThread* thread);
    void pushStackFrameStatic(ClassInfo* classInfo, MethodInfo* methodInfo, StackFrame* previousFrame, VMThread* thread);
    void pushStackFrameWithoutParams(ClassInfo* classInfo, MethodInfo* methodInfo, VMThread* thread);
    void pushStackFrameVirtual(ClassInfo* classInfo, MethodInfo* methodInfo, StackFrame* previousFrame, VMThread* thread);
    void runStaticInitializer(ClassInfo* classInfo, VMThread* thread);
public:
    void updateVariableFromVariable(Variable* variable, char* descriptor, Variable operand);
    VM();
    static uint16_t getDescriptorVarCount(char* get_string);
    void start(Configuration configuration);
    ClassInfo* getClass(const char* className, VMThread* thread);
    void runMain(const char* className);
};


