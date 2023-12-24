#include "VM.h"

#include "Configuration.h"
#include "Memory.h"
#include "Library/Builtin.h"

#include <stack>
#include <variant>

VM::VM(const Configuration configuration) noexcept
    : configuration(configuration)
{
}

void VM::start()
{
    Platform::initialize();

    registerBuiltinRegisterNatives();
    getClass("java/lang/OutOfMemoryError", &thread);
    getClass("java/lang/VirtualMachineError", &thread);
    getClass("java/lang/Object", &thread);
    getClass("java/lang/Class", &thread);
    getClass("java/lang/String", &thread);
    getClass("java/lang/System", &thread);
    getClass("Vigur/lang/System", &thread);
}

std::vector<Variable> VM::createVariableForDescriptor(const char* descriptor)
{
    std::vector<Variable> variables;
    if (strcmp(descriptor, "I") == 0)
    {
        constexpr Variable variable{VariableType_INT};
        variables.push_back(variable);
    } else if (strcmp(descriptor, "Z") == 0)
    {
        constexpr Variable variable{VariableType_BOOLEAN};
        variables.push_back(variable);
    } else if (strcmp(descriptor, "B") == 0)
    {
        constexpr Variable variable{VariableType_BYTE};
        variables.push_back(variable);
    } else if (strcmp(descriptor, "J") == 0)
    {
        constexpr Variable variableLow{VariableType_LONG};
        constexpr Variable variableHigh{VariableType_LONG};
        variables.push_back(variableHigh);
        variables.push_back(variableLow);
    } else if (descriptor[0] == '[')
    {
        constexpr Variable variable{VariableType_REFERENCE};
        variables.push_back(variable);
    } else if (descriptor[0] == 'L')
    {
        constexpr Variable variable{VariableType_REFERENCE};
        variables.push_back(variable);
    } else if (descriptor[0] == 'F') {
        constexpr Variable variable{VariableType_FLOAT};
        variables.push_back(variable);
    }
    else
    {
        fprintf(stderr, "Error: Couldn't construct data for descriptor type: %s\n", descriptor);
        Platform::exitProgram(7);
    }
    return variables;
}

u1 VM::getDescriptorVarCategory(const char* descriptor) noexcept
{
    // Longs and doubles use two
    if (descriptor[0] == 'J' || descriptor[0] == 'D')
    {
        return 2;
    }
    return 1;
}

void VM::initStaticFields(ClassInfo* class_info, VMThread* thread)
{
    // TODO: Do it for superclasses as well?
    u2 staticFieldsCount = 0;
    for (u2 currentField = 0; currentField < class_info->fieldsCount; ++currentField)
    {
        FieldInfo* field = class_info->fields[currentField];
        if (field->isStatic())
        {
            const u1 varCount = getDescriptorVarCategory(class_info->constantPool->getString(class_info->fields[currentField]->descriptorIndex));
            staticFieldsCount += varCount;
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
            const i4 index = static_cast<i4>(currentStaticField)-variables.size();
            if (index > staticFieldsCount-1 || index < 0)
            {
                thread->internalError("Going outside of index!");
            }
            field->staticData = &class_info->staticFields[index];
        }
    }
}

void VM::updateVariableFromVariable(Variable* variable, const char* descriptor, Variable operand, Variable operand2, VMThread* thread)
{
    if (strcmp(descriptor, "I") == 0)
    {
        checkType(*variable, VariableType_INT, thread);
        checkType(operand, VariableType_INT, thread);

        variable->data = operand.data;
    } else if (strcmp(descriptor, "Z") == 0)
    {
        checkType(*variable, VariableType_BOOLEAN, thread);
        checkType(operand, VariableType_INT, thread);

        variable->data = operand.data;
    } else if (descriptor[0] == '[' || descriptor[0] == 'L') {

        checkType(*variable, VariableType_REFERENCE, thread);
        checkType(operand, VariableType_REFERENCE, thread);

        variable->data = operand.data;
    } else if (descriptor[0] == 'J' || descriptor[0] == 'J')
    {
        checkType(*variable, VariableType_LONG, thread);
        checkType(operand, VariableType_LONG, thread);
        variable[0].data = operand2.data;
        variable[1].data = operand.data;
    } else if (descriptor[0] == 'D' || descriptor[0] == 'D')
    {
        checkType(*variable, VariableType_DOUBLE, thread);
        checkType(operand, VariableType_DOUBLE, thread);
        variable[0].data = operand2.data;
        variable[1].data = operand.data;
    } else
    {
        char buffer[200];
        snprintf(buffer, 200, "Error: Setting of variable of type with descriptor: %s not implemented yet!\n", descriptor);
        thread->internalError(buffer);
    }
}

static u1 readByte(VMThread* thread)
{
    return thread->currentMethod->code->code[thread->pc++];
}

[[maybe_unused]]
static u2 readShort(VMThread* thread)
{
    uint8_t indexByte1 = thread->currentMethod->code->code[thread->pc++];
    uint8_t indexByte2 = thread->currentMethod->code->code[thread->pc++];
    uint16_t shortCombined = (indexByte1 << 8) | indexByte2;
    return shortCombined;
}

void VM::executeLoop(VMThread* thread)
{
    while(!thread->stack.frames.empty())
    {
        uint8_t opcode = readByte(thread);
        printf("Running instruction with opcode: 0x%0x ", opcode);

        bool found = false;

        for (const Instruction& instruction : instructions) {
            if (((u1)instruction.opcode) == opcode)
            {
                found = true;
                printf("%s\n", instruction.name);
                uint8_t* args = 0;
                if (instruction.args > 0) {
                    args = (uint8_t*)Platform::allocateMemory(instruction.args, 0);
                    for (u2 currentArg = 0; currentArg < instruction.args; ++currentArg)
                    {
                        args[currentArg] = readByte(thread);
                    }
                }
                if (instruction.instructionFunction != NULL) {
                    instruction.instructionFunction(args, instruction.args, instruction.arg, &heap, thread, this);
                }
                break;
            }
        }

        if (found)[[likely]]
        {
            continue;
        } else [[unlikely]] {
            printf("\n");
            char buffer[200];
            snprintf(buffer, 200, "Unrecognized opcode detected: 0x%0x", opcode);
            thread->internalError(buffer);
        }
    }
}

void VM::runStaticInitializer(ClassInfo* classInfo, VMThread* thread)
{
    MethodInfo* entryPoint = classInfo->findMethodWithNameAndDescriptor("<clinit>", "()V");

    if (entryPoint == 0)
    {
        // No static initializers for this class
        return;
    }

    JavaStack oldStack = thread->stack;
    u4 oldPc = thread->pc;
    ClassInfo* oldCurrentClass = thread->currentClass;
    const MethodInfo* oldCurrentMethod = thread->currentMethod;
    StackFrame* oldFrame = thread->currentFrame;
    thread->stack.frames = std::vector<StackFrame>();
    thread->stack.frames.reserve(200);

    thread->pushStackFrameWithoutParams(classInfo, entryPoint);

    printf("Executing static initializers...\n");
    executeLoop(thread);

    thread->pc = oldPc;
    thread->currentClass = oldCurrentClass;
    thread->currentMethod = oldCurrentMethod;
    thread->stack.frames = oldStack.frames;
    thread->currentFrame = oldFrame;
}

ClassInfo* VM::getClass(const char* className, VMThread* thread)
{
    ClassInfo* classInfo = heap.getClassByName(className);
    if (classInfo == NULL) {
        Memory *memory = new Memory(MIB(1), MIB(30));
        printf("Loading class %s...\n", className);
        classInfo = bootstrapClassLoader.readClass(className, memory, configuration.classPath);
        initStaticFields(classInfo, thread);
        heap.addClassInfo(classInfo);
        runStaticInitializer(classInfo, thread);
        return classInfo;
    }
    return classInfo;
}

void VM::runMain(const char* className)
{
    VMThread* mainThread = &thread;
    if (className == 0)
    {
        fprintf(stderr, "Error: Class name of starting class not defined..\n");
        Platform::exitProgram(6);
    }

    Memory memory(1000, KIB(5));
    ClassInfo* startupClass = getClass(className, mainThread);
    MethodInfo* entryPoint = startupClass->findMethodWithNameAndDescriptor("main", "([Ljava/lang/String;)V");

    if (entryPoint == 0)
    {
        fprintf(stderr, "Error: Entry point not found. Exiting...\n");
        Platform::exitProgram(6);
    }

    mainThread->pc = 0;
    mainThread->currentClass = startupClass;
    mainThread->currentMethod = entryPoint;

    mainThread->pushStackFrameStatic(startupClass, entryPoint, 0);

    // TODO: Put string array in local variable with index 0

    printf("> Executing main method...\n");
    executeLoop(mainThread);
    // Object* object = heap.getObject(3);
    printf("> Done executing\n");
}

void VM::shutdown()
{
    Platform::cleanup();
}

void VM::checkType(const Variable var, const VariableType type, const VMThread* thread)
{
    if (var.type != type)
    {
        thread->internalError("Error: TypeMismatch");
    }
}
