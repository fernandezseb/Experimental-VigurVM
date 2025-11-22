#pragma once

#include "Core.h"
#include "Configuration.h"
#include "ClassLoader/ClassLoader.h"
#include "Data/Variable.h"
#include "Data/Opcode.h"
#include "JavaHeap.h"
#include "VMThread.h"

#include "Instructions/ComparisonInstructions.h"
#include "Instructions/ConstantInstructions.h"
#include "Instructions/ControlInstructions.h"
#include "Instructions/LoadInstructions.h"
#include "Instructions/ReferenceInstructions.h"
#include "Instructions/StoreInstructions.h"
#include "Instructions/StackInstructions.h"
#include "Instructions/ExtendedInstructions.h"
#include "Instructions/MathInstructions.h"
#include "Instructions/ConversionInstructions.h"

#include <vector>
#include <array>
#include <string>

class VM;

struct Instruction
{
    Opcode opcode;
    u1 args;
    std::string_view name;
    i1 arg;
    void(*instructionFunction)(const InstructionInput& input);
};


class VM {
public:
    explicit VM(Configuration configuration) noexcept;
    void updateVariableFromVariable(Variable* variable, std::string_view descriptor, Variable operand, Variable operand2, VMThread* thread);
    static std::vector<Variable> createVariableForDescriptor(std::string_view descriptor);
    [[nodiscard]] static u1 getDescriptorVarCategory(std::string_view descriptor) noexcept;
    void start(std::string_view commandLineName);
    ClassInfo* getClass(std::string_view className, VMThread* thread);
    void executeNativeMethod(const ClassInfo* targetClass, const MethodInfo* methodInfo, JavaHeap* heap, VMThread* thread);
    void runMain();
    void shutdown();
    void executeLoop(VMThread* thread);
    static void checkType(Variable var, VariableType type, VMThread *thread);
    VMThread* getVMThreadByObjectRef(const u4 objectReference)
    {
        if (m_mainThread.threadObject == objectReference)
        {
            return &m_mainThread;
        }

        return nullptr;
    }
    bool isSubclass(VMThread* thread, const ClassInfo* targetClass, ClassInfo* subClass);
    FieldInfo* findField(ClassInfo* classInfo, const char* name, const char* descriptor, VMThread* thread);
    std::string userDir;
private:
    void initSystemClass(ClassInfo* class_info, VMThread* vm_thread);
    static constexpr std::array<Instruction,136> m_instructions{{
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
        {i_lload, 1, "lload", 0, lload},
        {i_fload, 1, "fload", 0, fload},
        {i_aload, 1, "aload", 0, aload},
        {i_iload_0, 0, "iload_0", 0, iload_i},
        {i_iload_1, 0, "iload_1", 1, iload_i},
        {i_iload_2, 0, "iload_2", 2, iload_i},
        {i_iload_3, 0, "iload_3", 3, iload_i},
        {i_lload_0, 0, "lload_0", 0, lload_i},
        {i_lload_1, 0, "lload_1", 1, lload_i},
        {i_lload_2, 0, "lload_2", 2, lload_i},
        {i_lload_3, 0, "lload_3", 3, lload_i},
        {i_fload_0, 0, "fload_0", 0, fload_i},
        {i_fload_1, 0, "fload_1", 1, fload_i},
        {i_fload_2, 0, "fload_2", 2, fload_i},
        {i_fload_3, 0, "fload_3", 3, fload_i},
        {i_aload_0, 0, "aload_0", 0, aload_i},
        {i_aload_1, 0, "aload_1", 1, aload_i},
        {i_aload_2, 0, "aload_2", 2, aload_i},
        {i_aload_3, 0, "aload_3", 3, aload_i},
        {i_iaload, 0, "iaload", 0, iaload},
        {i_aaload, 0, "aaload", 0, aaload},
        {i_baload, 0, "baload", 0, baload},
        {i_caload, 0, "caload", 0, caload},
        // Stores
        {i_istore, 1, "istore", 0, istore},
        {i_fstore, 1, "fstore", 0, fstore},
        {i_astore, 1, "astore", 0, astore},
        {i_istore_0, 0, "astore_0", 0, istore_i},
        {i_istore_1, 0, "astore_1", 1, istore_i},
        {i_istore_2, 0, "astore_2", 2, istore_i},
        {i_istore_3, 0, "astore_3", 3, istore_i},
        {i_lstore_0, 0, "lstore_0", 0, lstore_i},
        {i_lstore_1, 0, "lstore_1", 1, lstore_i},
        {i_lstore_2, 0, "lstore_2", 2, lstore_i},
        {i_lstore_3, 0, "lstore_3", 3, lstore_i},
        {i_astore_0, 0, "astore_0", 0, astore_i},
        {i_astore_1, 0, "astore_1", 1, astore_i},
        {i_astore_2, 0, "astore_2", 2, astore_i},
        {i_astore_3, 0, "astore_3", 3, astore_i},
        {i_iastore, 0, "iastore", 0, iastore},
        {i_aastore, 0, "aastore", 0, aastore},
        {i_bastore, 0, "bastore", 0, bastore},
        {i_castore, 0, "castore", 0, castore},
        // Stack
        {i_pop, 0, "pop", 0, pop},
        {i_dup, 0, "dup", 0, dup},
        {i_dup_x1, 0, "dup_x1", 0, dup_x1},
        {i_dup2, 0, "dup2", 0, dup2},
        // Math
        {i_iadd, 0, "iadd", 0, iadd},
        {i_ladd, 0, "ladd", 0, ladd},
        {i_isub, 0, "isub", 0, isub},
        {i_imul, 0, "imul", 0, imul},
        {i_fmul, 0, "fmul", 0, fmul},
        {i_idiv, 0, "idiv", 0, idiv},
        {i_fdiv, 0, "fdiv", 0, fdiv},
        {i_irem, 0, "irem", 0, irem},
        {i_ineg, 0, "ineg", 0, ineg},
        {i_ishl, 0, "ishl", 0, ishl},
        {i_ishr, 0, "ishr", 0, ishr},
        {i_lshl, 0, "lshl", 0, lshl},
        {i_iushr, 0, "iushr", 0, iushr},
        {i_iand, 0, "iand", 0, iand},
        {i_land, 0, "land", 0, land},
        {i_ior, 0, "ior", 0, ior},
        {i_lor, 0, "lor", 0, lor},
        {i_ixor, 0, "ixor", 0, ixor},
        {i_iinc, 2, "iinc", 0, iinc},
        // Conversions
        {i_i2l, 0, "i2l", 0, i2l},
        {i_i2f, 0, "i2f", 0, i2f},
        {i_f2i, 0, "f2i", 0, f2i},
        {i_i2c, 0, "i2c", 0, i2c},
        // Comparisons
        {i_lcmp, 0, "lcmp", 0, lcmp},
        {i_fcmpl, 0, "fcmpl", 0, fcmpl},
        {i_fcmpg, 0, "fcmpg", 0, fcmpg},
        {i_ifeq, 2, "ifeq", 0, ifeq},
        {i_ifne, 2, "ifne", 0, ifne},
        {i_iflt, 2, "iflt", 0, iflt},
        {i_ifge, 2, "ifge", 0, ifge},
        {i_ifgt, 2, "ifgt", 0, ifgt},
        {i_ifle, 2, "ifle", 0, ifle},
        {i_if_icmpeq, 2, "if_icmpeq", 0, if_icmpeq},
        {i_if_icmpne, 2, "if_icmpne", 0, if_icmpne},
        {i_if_icmplt, 2, "if_icmplt", 0, if_icmplt},
        {i_if_icmpge, 2, "if_icmpge", 0, if_icmpge},
        {i_if_icmpgt, 2, "if_icmpgt", 0, if_icmpgt},
        {i_if_icmple, 2, "if_icmple", 0, if_icmple},
        {i_if_acmpeq, 2, "if_acmpeq", 0, if_acmpeq},
        {i_if_acmpne, 2, "if_acmpne", 0, if_acmpne},
        // References
        {i_getstatic, 0, "getstatic", 0, getstatic},
        {i_putstatic, 0, "putstatic", 0, putstatic},
        {i_getfield, 0, "getfield", 0, getfield},
        {i_putfield, 0, "putfield", 0, putfield},
        {i_invokevirtual, 0, "invokevirtual", 0, invokevirtual},
        {i_invokespecial, 0, "invokespecial", 0, invokespecial},
        {i_invokestatic, 0, "invokestatic", 0, invokestatic},
        {i_invokeinterface, 4, "invokeinterface", 0, invokeinterface},
        {i_new, 0, "new", 0, newInstruction},
        {i_newarray, 1, "newarray", 0, newarray},
        {i_anewarray, 0, "anewarray", 0, anewarray},
        {i_arraylength, 0, "arraylength", 0, arraylength},
        {i_athrow, 0, "athrow", 0, athrow},
        {i_checkcast, 2, "checkcast", 0, checkCast},
        {i_instanceof, 2, "instanceof", 0, instanceof},
        {i_monitorenter, 0, "monitorenter", 0, monitorenter},
        {i_monitorexit, 0, "monitorexit", 0, monitorexit},
        // Control
        {i_goto, 2, "goto", 0, gotoInstruction},
        {i_lookupswitch, 0, "lookupswitch", 0, lookupswitch},
        {i_ireturn, 0, "ireturn", 0, ireturnInstruction},
        {i_lreturn, 0, "lreturn", 0, lreturnInstruction},
        {i_freturn, 0, "freturn", 0, freturnInstruction},
        {i_dreturn, 0, "dreturn", 0, dreturnInstruction},
        {i_areturn, 0, "areturn", 0, areturnInstruction},
        {i_return, 0, "return", 0, returnInstruction},
        // Extended
        {i_wide, 0, "wide", 0, wide},
        {i_ifnull, 0, "ifnull", 0, ifnull},
        {i_ifnonnull, 0, "ifnonnull", 0, ifnonnull},
    }};
    ClassLoader m_bootstrapClassLoader;
    JavaHeap m_heap;
    VMThread m_mainThread{"main", 200};
    Configuration m_configuration;
    void initStaticFields(ClassInfo* class_info, VMThread* thread);
    void runStaticInitializer(ClassInfo* classInfo, VMThread* thread);
    u4 createThreadObject(VMThread* thread, u4 threadGroupReference);
    u4 createThreadGroupObject(VMThread* thread);
    void createArgsArray(const VMThread* thread);
};


