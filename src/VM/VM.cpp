#include "VM.h"

#include "Configuration.h"
#include "../Memory.h"

VM::VM()
{
}

void VM::start(Configuration configuration)
{
    this->configuration = configuration;
    getClass("java/lang/OutOfMemoryError");
    getClass("java/lang/VirtualMachineError");
    getClass("java/lang/Object");
    getClass("java/lang/String");
}

ClassInfo* VM::getClassByName(const char* class_name)
{
    for (size_t currentClass = 0; currentClass < heap.methodArea.classes.getSize() ; currentClass++)
    {
        ClassInfo* classInfo = heap.methodArea.classes.get(currentClass);
        if (strcmp(classInfo->getName(), class_name) == 0)
        {
            return classInfo;
        }
    }
    return 0;
}

std::vector<Variable> VM::createVariableForDescriptor(char* descriptor)
{
    std::vector<Variable> variables;
    if (strcmp(descriptor, "I") == 0)
    {
        Variable variable;
        variable.type = VariableType_INT;
        variable.data = ((int32_t)0);
        variables.push_back(variable);
    } else if (strcmp(descriptor, "B") == 0)
    {
        Variable variable;
        variable.type = VariableType_BYTE;
        variable.data = ((int32_t)0);
        variables.push_back(variable);
    } else if (strcmp(descriptor, "J") == 0)
    {
        Variable variableLow;
        variableLow.type = VariableType_LONG;
        variableLow.data = ((int64_t) 0);

        Variable variableHigh;
        variableHigh.type = VariableType_LONG;
        variableHigh.data = (((int64_t) 0) << 32);
        variables.push_back(variableHigh);
        variables.push_back(variableLow);
    }
    else
    {
        fprintf(stderr, "Error: Couldn't construct data for descriptor type: %s\n", descriptor);
        Platform::exitProgram(-7);
    }
    return variables;
}

uint16_t VM::getDescriptorVarCount(char* descriptor)
{
    // Longs and doubles use two
    if (descriptor[0] == 'J' || descriptor[0] == 'D')
    {
        return 2;
    }
    return 1;
}

void VM::initStaticFields(ClassInfo* class_info)
{
    u2 staticFieldsCount = 0;
    for (u2 currentField = 0; currentField < class_info->fieldsCount; ++currentField)
    {
        FieldInfo* field = class_info->fields[currentField];
        if (field->isStatic())
        {
            staticFieldsCount += getDescriptorVarCount(class_info->constantPool->getString(class_info->fields[currentField]->descriptorIndex));
        }
    }

    class_info->staticFieldsCount = staticFieldsCount;
    class_info->staticFields = (Variable*) class_info->memory->alloc(sizeof(Variable) * staticFieldsCount);

    u2 currentStaticField = 0;
    for (u2 currentField = 0; currentField < class_info->fieldsCount; ++currentField)
    {
        FieldInfo* field = class_info->fields[currentField];
        if (field->isStatic())
        {
            std::vector<Variable> variables = createVariableForDescriptor(class_info->constantPool->getString(field->descriptorIndex));
            for (Variable variable : variables) {
                class_info->staticFields[currentStaticField++] = variable;
            }
            field->staticData = &class_info->staticFields[currentStaticField-variables.size()];
        }
    }
}

ClassInfo* VM::getClass(const char* className)
{
    ClassInfo* classInfo = getClassByName(className);
    if (classInfo == NULL) {
        Memory *memory = new Memory(2000, MIB(20));
        printf("Loading class %s...\n", className);
        ClassInfo *classInfo = bootstrapClassLoader.readClass(className, memory, configuration.classPath);
        initStaticFields(classInfo);
        // TODO: Run static initializers (clinit)
        heap.methodArea.classes.add(classInfo);
        return classInfo;
    }
    return classInfo;
}

void VM::runMain(const char* className)
{
    if (className == 0)
    {
        fprintf(stderr, "Error: Class name of starting class not defined..\n");
        Platform::exitProgram(-6);
    }

    Memory memory(1000, KIB(5));
    ClassInfo* startupClass = getClass(className);
    MethodInfo* entryPoint = startupClass->findMethodWithName("main");

    if (entryPoint == 0)
    {
        fprintf(stderr, "Error: Entry point not found. Exiting...\n");
        Platform::exitProgram(-6);
    }

    thread.pc = 0;
    thread.currentClass = startupClass;
    thread.currentMethod = entryPoint;
    thread.name = "main";
    thread.stack.frames.reserve(200);

    StackFrame stackFrame;
    stackFrame.localVariables.reserve(entryPoint->code->maxLocals);
    stackFrame.operands.reserve(entryPoint->code->maxStack);
    stackFrame.constantPool = startupClass->constantPool;
    stackFrame.previousPc = 0;

    thread.stack.frames.push_back(stackFrame);
    thread.currentFrame = &thread.stack.frames[0];

    printf("brol");
}
