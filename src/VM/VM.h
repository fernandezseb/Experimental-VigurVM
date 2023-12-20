#pragma once

#include "Core.h"
#include "Configuration.h"
#include "ClassLoader/ClassLoader.h"
#include "Data/Variable.h"
#include "Data/Opcode.h"
#include "JavaHeap.h"

#include <vector>

#include "Instructions/ComparisonInstructions.h"
#include "Instructions/ConstantInstructions.h"
#include "Instructions/ControlInstructions.h"
#include "Instructions/LoadInstructions.h"
#include "Instructions/ReferenceInstructions.h"
#include "Instructions/StoreInstructions.h"
#include "Instructions/StackInstructions.h"
#include "Instructions/ExtendedInstructions.h"
#include "Instructions/MathInstructions.h"

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

    const char* className;
    const char* methodName;

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
    const char* name;
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
    Instruction instructions[78] =
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
        {i_iload, 1, "iload", 0, iload},
        {i_iload_0, 0, "iload_0", 0, iload_i},
        {i_iload_1, 0, "iload_1", 1, iload_i},
        {i_iload_2, 0, "iload_2", 2, iload_i},
        {i_iload_3, 0, "iload_3", 3, iload_i},
        {i_aload_0, 0, "aload_0", 0, aload_i},
        {i_aload_1, 0, "aload_1", 1, aload_i},
        {i_aload_2, 0, "aload_2", 2, aload_i},
        {i_aload_3, 0, "aload_3", 3, aload_i},
        {i_iaload, 0, "iaload", 0, iaload},
        {i_caload, 0, "caload", 0, caload},
        // Stores
        {i_istore, 1, "istore", 0, istore},
        {i_istore_0, 0, "astore_0", 0, istore_i},
        {i_istore_1, 0, "astore_1", 1, istore_i},
        {i_istore_2, 0, "astore_2", 2, istore_i},
        {i_istore_3, 0, "astore_3", 3, istore_i},
        {i_astore_0, 0, "astore_0", 0, astore_i},
        {i_astore_1, 0, "astore_1", 1, astore_i},
        {i_astore_2, 0, "astore_2", 2, astore_i},
        {i_astore_3, 0, "astore_3", 3, astore_i},
        {i_iastore, 0, "iastore", 0, iastore},
        {i_castore, 0, "castore", 0, castore},
        // Stack
        {i_pop, 0, "pop", 0, pop},
        {i_dup, 0, "dup", 0, dup},
        // Math
        {i_iadd, 0, "iadd", 0, iadd},
        {i_isub, 0, "isub", 0, isub},
        {i_imul, 0, "imul", 0, imul},
        {i_ishl, 0, "ishl", 0, ishl},
        {i_iushr, 0, "iushr", 0, iushr},
        {i_iinc, 2, "iinc", 0, iinc},
        // Comparisons
        {i_ifeq, 2, "ifeq", 0, ifeq},
        {i_ifne, 2, "ifne", 0, ifne},
        {i_ifge, 2, "ifge", 0, ifge},
        {i_ifgt, 2, "ifgt", 0, ifgt},
        {i_ifle, 2, "ifle", 0, ifle},
        {i_if_icmpne, 2, "if_icmpne", 0, if_icmpne},
        {i_if_icmplt, 2, "if_icmplt", 0, if_icmplt},
        {i_if_icmpgt, 2, "if_icmpgt", 0, if_icmpgt},
        {i_if_icmple, 2, "if_icmple", 0, if_icmple},
        // References
        {i_getstatic, 0, "getstatic", 0, getstatic},
        {i_putstatic, 0, "putstatic", 0, putstatic},
        {i_getfield, 0, "getfield", 0, getfield},
        {i_putfield, 0, "putfield", 0, putfield},
        {i_invokevirtual, 0, "invokevirtual", 0, invokevirtual},
        {i_invokespecial, 0, "invokespecial", 0, invokespecial},
        {i_invokestatic, 0, "invokestatic", 0, invokestatic},
        {i_new, 0, "new", 0, newInstruction},
        {i_newarray, 1, "newarray", 0, newarray},
        {i_anewarray, 0, "anewarray", 0, anewarray},
        {i_arraylength, 0, "arraylength", 0, arraylength},
        {i_monitorenter, 0, "monitorenter", 0, monitorenter},
        {i_monitorexit, 0, "monitorexit", 0, monitorexit},
        // Control
        {i_goto, 2, "goto", 0, gotoInstruction},
        {i_ireturn, 0, "ireturn", 0, ireturnInstruction},
        {i_areturn, 0, "areturn", 0, areturnInstruction},
        {i_return, 0, "return", 0, returnInstruction},
        // Extended
        {i_ifnonnull, 0, "ifnonnull", 0, ifnonnull},
    };
    ClassLoader bootstrapClassLoader;
    JavaHeap heap;
    VMThread thread;
    Configuration configuration;
    void initStaticFields(ClassInfo* class_info);
    void executeLoop(VMThread* thread);
    void pushStackFrameWithoutParams(ClassInfo* classInfo, MethodInfo* methodInfo, VMThread* thread);
    void runStaticInitializer(ClassInfo* classInfo, VMThread* thread);
public:
    void pushStackFrameStatic(ClassInfo* classInfo, MethodInfo* methodInfo, StackFrame* previousFrame, VMThread* thread);
    void pushStackFrameVirtual(ClassInfo* classInfo, MethodInfo* methodInfo, StackFrame* previousFrame, VMThread* thread);
    void updateVariableFromVariable(Variable* variable, const char* descriptor, Variable operand, Variable operand2);
    VM();
    static std::vector<Variable> createVariableForDescriptor(const char* descriptor);
    [[nodiscard]] static u1 getDescriptorVarCategory(const char* descriptor) noexcept;
    void start(Configuration configuration);
    ClassInfo* getClass(const char* className, VMThread* thread);
    void runMain(const char* className);
    void shutdown();
    void checkType(Variable var, VariableType type);
    void internalError(const char* error);
};


