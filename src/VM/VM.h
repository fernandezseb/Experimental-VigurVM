#pragma once

#include "Core.h"
#include "DynamicArray.h"
#include "Configuration.h"
#include "ClassLoader/ClassLoader.h"

#include <vector>


class MethodArea {
public:
    MethodArea() : classes(1000) {}
    // Loaded classes to use
    DynamicArray<ClassInfo*> classes;
};

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
};

class JavaStack {
public:
    std::vector<StackFrame> frames;
};

class JavaHeap {
public:
    MethodArea methodArea;
    // List of objects
    std::vector<Reference*> objects;
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

// METHOD INVOCATION USES LOCAL VARIABLES
// ON INSTANCE METHOD INVOCATION THE FIRST LOCAL VARIABLE IS THE IS REFERENCE TO THE OBJECT


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
    void runStaticInitializer(ClassInfo* class_info);
public:
    VM();
    void start(Configuration configuration);
    ClassInfo* getClassByName(const char* class_name);
    ClassInfo* getClass(const char* className);
    void runMain(const char* className);
};
