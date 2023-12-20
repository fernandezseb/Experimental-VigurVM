#include "VM.h"

#include <stack>
#include <variant>

#include "Configuration.h"
#include "../Memory.h"

VM::VM()
{
}

void VM::start(Configuration configuration)
{
    Platform::initialize();
    this->configuration = configuration;

    thread.name = "main";
    thread.stack.frames.reserve(200);
    thread.pc = 0;
    thread.currentClass = nullptr;
    thread.currentMethod = nullptr;
    thread.currentFrame = nullptr;

    getClass("java/lang/OutOfMemoryError", &thread);
    getClass("java/lang/VirtualMachineError", &thread);
    getClass("java/lang/Object", &thread);
    getClass("java/lang/String", &thread);
    getClass("java/lang/System", &thread);
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
    }
    else
    {
        fprintf(stderr, "Error: Couldn't construct data for descriptor type: %s\n", descriptor);
        Platform::exitProgram(-7);
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

void VM::initStaticFields(ClassInfo* class_info)
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
            const i4 index = currentStaticField-variables.size();
            if (index > staticFieldsCount-1 || index < 0)
            {
                internalError("Going outside of index!");
            }
            field->staticData = &class_info->staticFields[index];
        }
    }
}

void VM::updateVariableFromVariable(Variable* variable, const char* descriptor, Variable operand, Variable operand2)
{
    if (strcmp(descriptor, "I") == 0)
    {
        checkType(*variable, VariableType_INT);
        checkType(operand, VariableType_INT);

        variable->data = operand.data;
    } else if (strcmp(descriptor, "Z") == 0)
    {
        checkType(*variable, VariableType_BOOLEAN);
        checkType(operand, VariableType_INT);

        variable->data = operand.data;
    } else if (descriptor[0] == '[' || descriptor[0] == 'L') {

        checkType(*variable, VariableType_REFERENCE);
        checkType(operand, VariableType_REFERENCE);

        variable->data = operand.data;
    } else if (descriptor[0] == 'J' || descriptor[0] == 'J')
    {
        checkType(*variable, VariableType_LONG);
        checkType(operand, VariableType_LONG);
        variable[0].data = operand2.data;
        variable[1].data = operand.data;
    } else if (descriptor[0] == 'D' || descriptor[0] == 'D')
    {
        checkType(*variable, VariableType_DOUBLE);
        checkType(operand, VariableType_DOUBLE);
        variable[0].data = operand2.data;
        variable[1].data = operand.data;
    } else
    {
        char buffer[200];
        snprintf(buffer, 200, "Error: Setting of variable of type with descriptor: %s not implemented yet!\n", descriptor);
        internalError(buffer);
    }
}

static u1 readByte(VMThread* thread)
{
    return thread->currentMethod->code->code[thread->pc++];
}

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

        if (found)
        {
            continue;
        } else
        {
            printf("\n");
        }
        char buffer[200];
        snprintf(buffer, 200, "Unrecognized opcode detected: 0x%0x", opcode);
        internalError(buffer);
    }
}

void VM::pushStackFrameWithoutParams(ClassInfo* classInfo, MethodInfo* methodInfo, VMThread* thread)
{
    StackFrame stackFrame;
    stackFrame.localVariables.reserve(methodInfo->code->maxLocals);
    for (u2  currentLocal = 0; currentLocal < methodInfo->code->maxLocals; ++currentLocal)
    {
        constexpr Variable var{VariableType_UNDEFINED};
        stackFrame.localVariables.push_back(var);
    }
    stackFrame.operands.reserve(methodInfo->code->maxStack);
    stackFrame.constantPool = classInfo->constantPool;
    stackFrame.previousPc = thread->pc;
    stackFrame.previousClass = thread->currentClass;
    stackFrame.previousMethod = thread->currentMethod;
    stackFrame.className = classInfo->getName();
    stackFrame.methodName = methodInfo->name;


    thread->pc = 0;
    thread->currentClass = classInfo;
    thread->currentMethod = methodInfo;

    thread->stack.frames.push_back(stackFrame);
    thread->currentFrame = &thread->stack.frames[thread->stack.frames.size()-1];
}

void VM::pushStackFrameVirtual(ClassInfo* classInfo, MethodInfo* methodInfo, StackFrame* previousFrame, VMThread* thread)
{
    pushStackFrameWithoutParams(classInfo, methodInfo, thread);
    if (previousFrame != 0)
    {
        // The arguments and the pointer to the object
        for (int i = methodInfo->argsCount; i >= 0; --i)
        {
            thread->currentFrame->localVariables[i] = previousFrame->popOperand();
        }
    }
}

void VM::pushStackFrameStatic(ClassInfo* classInfo, MethodInfo* methodInfo, StackFrame* previousFrame, VMThread* thread)
{
    pushStackFrameWithoutParams(classInfo, methodInfo, thread);

    if (previousFrame != 0)
    {
        // The arguments
        for (int i = methodInfo->argsCount; i > 0; --i)
        {
            thread->currentFrame->localVariables[i-1] = previousFrame->popOperand();
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
    MethodInfo* oldCurrentMethod = thread->currentMethod;
    StackFrame* oldFrame = thread->currentFrame;
    thread->stack.frames = std::vector<StackFrame>();
    thread->stack.frames.reserve(200);

    pushStackFrameWithoutParams(classInfo, entryPoint, thread);

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
        ClassInfo *classInfo = bootstrapClassLoader.readClass(className, memory, configuration.classPath);
        initStaticFields(classInfo);
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
        Platform::exitProgram(-6);
    }

    Memory memory(1000, KIB(5));
    ClassInfo* startupClass = getClass(className, mainThread);
    MethodInfo* entryPoint = startupClass->findMethodWithNameAndDescriptor("main", "([Ljava/lang/String;)V");

    if (entryPoint == 0)
    {
        fprintf(stderr, "Error: Entry point not found. Exiting...\n");
        Platform::exitProgram(-6);
    }

    mainThread->pc = 0;
    mainThread->currentClass = startupClass;
    mainThread->currentMethod = entryPoint;

    pushStackFrameStatic(startupClass, entryPoint, 0, mainThread);

    // TODO: Put string array in local variable with index 0

    printf("> Executing main method...\n");
    executeLoop(mainThread);
    Object* object = heap.getObject(3);
    printf("> Done executing\n");
}

void VM::shutdown()
{
    Platform::cleanup();
}

void VM::checkType(Variable var, VariableType type)
{
    if (var.type != type)
    {
        internalError("Error: TypeMismatch");
    }
}

void VM::internalError(const char* error)
{
    fprintf(stdout, "Error: %s\n", error);
    if (thread.stack.frames.size() > 0)
    {
        for (i4 currentFrame = thread.stack.frames.size()-1; currentFrame >= 0; --currentFrame)
        {
            StackFrame frame = thread.stack.frames[currentFrame];
            printf("    at %s.%s\n", frame.className, frame.methodName);
        }
    }
    Platform::exitProgram(-6);
}
