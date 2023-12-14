#include "VM.h"

#include <stack>
#include <variant>

#include "Configuration.h"
#include "../Memory.h"
#include "ConstantInstructions.h"

VM::VM()
{
}

void VM::start(Configuration configuration)
{
    this->configuration = configuration;

    thread.name = "main";
    thread.stack.frames.reserve(200);
    thread.pc = 0;
    thread.currentClass = nullptr;
    thread.currentMethod = nullptr;

    getClass("java/lang/OutOfMemoryError", &thread);
    getClass("java/lang/VirtualMachineError", &thread);
    getClass("java/lang/Object", &thread);
    getClass("java/lang/String", &thread);
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
    } else if (descriptor[0] == '[')
    {
        Variable variable;
        variable.type = VariableType_REFERENCE;
        variable.data = 0;
        variables.push_back(variable);
    } else if (descriptor[0] == 'L')
    {
        Variable variable;
        variable.type = VariableType_REFERENCE;
        variable.data = 0;
        variables.push_back(variable);
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
    // TODO: Do it recursive as well
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

void VM::updateVariableFromVariable(Variable* variable, char* descriptor, Variable operand)
{
    // TODO: implement setting of field data for double and long
    if (strcmp(descriptor, "I") == 0)
    {
        if (variable->type != VariableType_INT)
        {
            fprintf(stderr, "Error: Type mismatch!\n");
            Platform::exitProgram(-78);
        }

        if (operand.type != VariableType_INT)
        {
            fprintf(stderr, "Error: Operand on stack is of the wrong type!\n");
            Platform::exitProgram(-90);
        }

        variable->data = operand.data;
        variable->type = VariableType_INT;
    } else if (descriptor[0] == '[' || descriptor[0] == 'L') {

        if (variable->type != VariableType_REFERENCE)
        {
            fprintf(stderr, "Error: Type mismatch!\n");
            Platform::exitProgram(-78);
        }

        if (operand.type != VariableType_REFERENCE)
        {
            fprintf(stderr, "Error: Operand on stack is of the wrong type!\n");
            Platform::exitProgram(-90);
        }
        variable->data = operand.data;
    } else
    {
        fprintf(stderr, "Error: Setting of variable of type with descriptor: %s not implemented yet!\n", descriptor);
        Platform::exitProgram(-6);
    }
}

void VM::executeLoop(VMThread* thread)
{
    while(!thread->stack.frames.empty())
    {
        StackFrame* topFrame = thread->currentFrame;
        uint8_t* code = thread->currentMethod->code->code;
        uint8_t opcode = code[thread->pc++];
        printf("Running instruction with opcode: 0x%0x\n", opcode);

        bool found = false;

        for (const Instruction& instruction : instructions) {
            if (((u1)instruction.opcode) == opcode)
            {
                uint8_t* args = 0;
                if (instruction.args > 0) {
                    args = (uint8_t*)Platform::allocateMemory(instruction.args, 0);
                    for (u2 currentArg = 0; currentArg < instruction.args; ++currentArg)
                    {
                        args[currentArg] = code[thread->pc++];
                    }
                }
                if (instruction.instructionFunction != NULL) {
                    found = true;
                    instruction.instructionFunction(args, instruction.args, instruction.arg, &heap, thread);
                    break;
                }
            }
        }
        if (found)
        {
            continue;
        }

        switch (opcode)
        {
        case 0x2a: // aload_0
            {
                // TODO: Check if it is OK type and such
                Variable var = topFrame->localVariables[0];
                topFrame->operands.push_back(var);
                break;
            }
        case 0x2b: // aload_1
            {
                // TODO: Check if it is OK type and such
                Variable var = topFrame->localVariables[1];
                topFrame->operands.push_back(var);
                break;
            }
        case 0x4c: // astore_1
            {
                Variable refVar = topFrame->popOperand();
                Variable* var =  &topFrame->localVariables[1];
                var->data = refVar.data;
                var->type = refVar.type;
                break;
            }
        case 0x59: // dup
            {
                Variable top = topFrame->peekOperand();
                Variable copy = {};
                copy.type = top.type;
                copy.data = top.data;
                topFrame->operands.push_back(copy);
                break;
            }
        case 0xb1: // return
            {
                StackFrame* stackFrame = &thread->stack.frames.back();
                thread->pc = stackFrame->previousPc;
                thread->currentClass = stackFrame->previousClass;
                thread->currentMethod = stackFrame->previousMethod;
                thread->stack.frames.pop_back();
                if (thread->stack.frames.size() > 0)
                {
                    StackFrame* previousStackFrame = &thread->stack.frames[thread->stack.frames.size()-1];
                    thread->currentFrame = previousStackFrame;
                } else
                {
                    thread->currentFrame = 0;
                }
                return;
            }
        case 0xb3: // putstatic
            {
                uint8_t indexByte1 = code[thread->pc++];
                uint8_t indexByte2 = code[thread->pc++];
                uint16_t index = (indexByte1 << 8) | indexByte2;
                CPFieldRef* fieldRef =  (CPFieldRef*) topFrame->constantPool->constants[index-1];
                CPClassInfo* classInfo =  topFrame->constantPool->getClassInfo(fieldRef->classIndex);
                CPNameAndTypeInfo* nameAndType = (CPNameAndTypeInfo*) topFrame->constantPool->constants[fieldRef->nameAndTypeIndex-1];
                const char* className = topFrame->constantPool->getString(classInfo->nameIndex);
                ClassInfo* targetClass = getClass(className, thread);
                FieldInfo* targetField = targetClass->findField(topFrame->constantPool->getString(nameAndType->nameIndex), topFrame->constantPool->getString(nameAndType->descriptorIndex));
                Variable var = topFrame->popOperand();
                updateVariableFromVariable(targetField->staticData, topFrame->constantPool->getString(nameAndType->descriptorIndex), var);
                break;
            }
        case 0xb5: // Putfield
            {
                uint8_t indexByte1 = code[thread->pc++];
                uint8_t indexByte2 = code[thread->pc++];
                uint16_t index = (indexByte1 << 8) | indexByte2;

                CPFieldRef* fieldRef = (CPFieldRef*) topFrame->constantPool->constants[index - 1];
                CPClassInfo* cpClassInfo = topFrame->constantPool->getClassInfo(fieldRef->classIndex);
                CPNameAndTypeInfo* nameAndType = (CPNameAndTypeInfo*) topFrame->constantPool->constants[fieldRef->nameAndTypeIndex-1];

                const char* className = topFrame->constantPool->getString(cpClassInfo->nameIndex);
                ClassInfo* targetClass = getClass(className, thread);
                const char* fieldName = topFrame->constantPool->getString(nameAndType->nameIndex);
                const char* fieldDescr = topFrame->constantPool->getString(nameAndType->descriptorIndex);


                // TODO: Get the object from the reference -> Done
                // TODO: Get the correct field from the object -> Done
                // TODO: Update the value of the object -> Basic Done

                Variable targetValue = topFrame->popOperand();
                Variable referencePointer = topFrame->popOperand();

                Object* targetObject = heap.getObject(referencePointer.data);

                FieldData* fieldData = targetObject->getField(fieldName, fieldDescr);

                if (targetValue.type != VariableType_INT)
                {
                    fprintf(stderr, "Error: Can't set fields other than int!");
                    Platform::exitProgram(-32);
                }
                // TODO: Fix for types other than I
                // TODO: Check for type descriptor
                fieldData->data.data = targetValue.data;
                fieldData->data.type = targetValue.type;

                break;
            }
        case 0xb7: // Invoke special
            {
                uint8_t indexByte1 = code[thread->pc++];
                uint8_t indexByte2 = code[thread->pc++];
                uint16_t index = (indexByte1 << 8) | indexByte2;
                CPMethodRef* methodRef = (CPMethodRef*) topFrame->constantPool->constants[index-1];
                CPClassInfo* cpClassInfo = topFrame->constantPool->getClassInfo(methodRef->classIndex);
                CPNameAndTypeInfo* nameAndTypeInfo = (CPNameAndTypeInfo*) topFrame->constantPool->constants[methodRef->nameAndTypeIndex-1];
                const char* methodName = topFrame->constantPool->getString(nameAndTypeInfo->nameIndex);
                const char* methodDescriptor = topFrame->constantPool->getString(nameAndTypeInfo->descriptorIndex);
                const char* className = topFrame->constantPool->getString(cpClassInfo->nameIndex);
                ClassInfo* targetClassInfo = getClass(className, thread);
                MethodInfo* methodInfo = targetClassInfo->findMethodWithNameAndDescriptor(methodName, methodDescriptor);
                // TODO: Implement argument passing (including subclass argument)
                // TODO: Check correct parsing of descriptors with subclasses

                if (strcmp(methodName, "<init>") ==0)
                {
                    // TODO: Check argument types
                    pushStackFrameVirtual(targetClassInfo, methodInfo, topFrame, thread);

                    printf("> Created new stack frame for constructor call on: %s\n", className);
                } else
                {
                    fprintf(stderr, "Error: Invokespecial not implemented for other cases than constructors\n");
                    Platform::exitProgram(-30);
                }

                break;
            }
        case 0xb8: // invoke static
            {
                uint8_t indexByte1 = code[thread->pc++];
                uint8_t indexByte2 = code[thread->pc++];
                uint16_t index = (indexByte1 << 8) | indexByte2;
                CPMethodRef* methodRef = (CPMethodRef*) topFrame->constantPool->constants[index-1];
                CPClassInfo* targetClassInfo = topFrame->constantPool->getClassInfo(methodRef->classIndex);
                CPNameAndTypeInfo* nameAndTypeInfo = (CPNameAndTypeInfo*) topFrame->constantPool->constants[methodRef->nameAndTypeIndex-1];
                ClassInfo* targetClass = getClass(topFrame->constantPool->getString(targetClassInfo->nameIndex), thread);
                // TODO: Take in account descriptor of method as well, for overriding and such
                MethodInfo* methodInfo = targetClass->findMethodWithName(topFrame->constantPool->getString(nameAndTypeInfo->nameIndex));

                if (!methodInfo->isStatic())
                {
                    // TODO: Do method call by pushing stack frame
                    fprintf(stderr, "Error: Running non-static method as static\n");
                    Platform::exitProgram(-10);
                    pushStackFrameStatic(targetClass, methodInfo, topFrame, thread);
                }

                if (methodInfo->isNative())
                {
                    printf("Running native code of method: %s\n", methodInfo->name);
                } else
                {
                    fprintf(stderr, "Error: Running static methods not implemented yet\n");
                    Platform::exitProgram(-10);
                }
                break;
            }
        case 0xbb: // new
            {
                uint8_t indexByte1 = code[thread->pc++];
                uint8_t indexByte2 = code[thread->pc++];
                uint16_t index = (indexByte1 << 8) | indexByte2;

                CPClassInfo* cpClasInfo = topFrame->constantPool->getClassInfo(index);
                ClassInfo* targetClass = getClass(topFrame->constantPool->getString(cpClasInfo->nameIndex), thread);

                uint32_t reference = heap.createObject(targetClass);
                Variable variable = {};
                variable.type = VariableType_REFERENCE;
                variable.data = reference;

                topFrame->operands.push_back(variable);

                break;
            }
        case 0xbd: // anewarray
            {
                uint8_t indexByte1 = code[thread->pc++];
                uint8_t indexByte2 = code[thread->pc++];
                uint16_t index = (indexByte1 << 8) | indexByte2;

                CPClassInfo* cpclassInfo = topFrame->constantPool->getClassInfo(index);
                ClassInfo* classInfo = getClass(topFrame->constantPool->getString(cpclassInfo->nameIndex), thread);

                int32_t size = topFrame->popOperand().data;

                if (size < 0)
                {
                    fprintf(stderr, "Error: Can't create an array with negative size\n");
                    Platform::exitProgram(-6);
                }

                uint32_t reference = heap.createArray(AT_REFERENCE, size);
                Variable variable;
                variable.type = VariableType_REFERENCE;
                variable.data = reference;

                topFrame->operands.push_back(variable);
                break;
            }
        default:
            {
                fprintf(stderr, "Unrecognized opcode detected: 0x%0x", opcode);
                Platform::exitProgram(-3);
            }
        }
    }
}

void VM::pushStackFrameWithoutParams(ClassInfo* classInfo, MethodInfo* methodInfo, VMThread* thread)
{
    StackFrame stackFrame;
    stackFrame.localVariables.reserve(methodInfo->code->maxLocals);
    for (u2  currentLocal = 0; currentLocal < methodInfo->code->maxLocals; ++currentLocal)
    {
        Variable var = {};
        var.type = VariableType_UNDEFINED;
        var.data = 0;
        stackFrame.localVariables.push_back(var);
    }
    stackFrame.operands.reserve(methodInfo->code->maxStack);
    stackFrame.constantPool = classInfo->constantPool;
    stackFrame.previousPc = thread->pc;
    stackFrame.previousClass = thread->currentClass;
    stackFrame.previousMethod = thread->currentMethod;


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

    pushStackFrameWithoutParams(classInfo, entryPoint, thread);

    printf("Executing static initializers...\n");
    executeLoop(thread);
}

ClassInfo* VM::getClass(const char* className, VMThread* thread)
{
    ClassInfo* classInfo = heap.getClassByName(className);
    if (classInfo == NULL) {
        Memory *memory = new Memory(2000, MIB(20));
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
    ClassInfo* clasInfo = heap.getClassByName("Brol");
    printf("> Done executing\n");
}
