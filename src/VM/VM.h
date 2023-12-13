#pragma once

#include "Core.h"
#include "DynamicArray.h"
#include "Configuration.h"
#include "ClassLoader/ClassLoader.h"
#include "Data/Variable.h"

#include <vector>

enum ReferenceType : char {
    OBJECT, ARRAY
};

class Object;
class Array;

class Reference {
public:
    Reference(ReferenceType type) { this->type = type; };
    ReferenceType type;

    Array* getArray() {
        if (type == ARRAY) {
            return (Array*)this;
        }
        else {
            fprintf(stderr, "Error: Reference is not of type 'Array'");
        }
    };

    Object* getObject() {
        if (type == OBJECT) {
            return (Object*)this;
        }
        else {
            fprintf(stderr, "Error: Reference is not of type 'Object'");
        }
    };
};

class FieldData {
public:
    uint16_t nameIndex;
    uint16_t descriptorIndex;
    Variable data; // TODO: Implement this for longs
};

// TODO: Add superclass references
class Object : public Reference {
public:
    Object() : Reference(OBJECT) {
    };
    FieldData* fields;
    uint16_t fieldsCount;
    ClassInfo* classInfo;
    FieldData* getField(const char* name, const char* descriptor)
    {
        for (u2 currentField = 0; currentField < fieldsCount; ++currentField)
        {
            const char* targetName = classInfo->constantPool->getString(fields[currentField].nameIndex);
            const char* targetDescriptor = classInfo->constantPool->getString(fields[currentField].descriptorIndex);

            if (strcmp(name, targetName) == 0 && strcmp(targetDescriptor, descriptor) == 0)
            {
                return &fields[currentField];
            }
        }
        return 0;
    }
};

class Array : public Reference {
public:
    Array() : Reference(ARRAY), arrayType(AT_UNDEFINED), length(0l), data(0) {
    };
    ArrayType arrayType;
    uint64_t length;
    uint32_t* data;
};

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

    ClassInfo* previousClass;
    MethodInfo* previousMethod;
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

    uint32_t createArray(ArrayType type, uint64_t size);
    uint32_t createObject(ClassInfo* class_info);
    Object* getObject(uint32_t id);
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
