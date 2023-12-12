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
    this->configuration = configuration;

    thread.name = "main";
    thread.stack.frames.reserve(200);
    thread.pc = 0;
    thread.currentClass = nullptr;
    thread.currentMethod = nullptr;

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

uint32_t JavaHeap::createArray(ArrayType type, uint64_t size)
{
    Array* array = (Array*) Platform::allocateMemory(sizeof(Array), 0);
    array->length = size;
    array->type = ARRAY;
    array->arrayType = AT_REFERENCE;
    array->data = (uint32_t*) Platform::allocateMemory(sizeof(uint32_t) * size, 0);
    for (int i = 0; i < size; ++i)
    {
        array->data[i] = 0;
    }
    objects.push_back(array);
    return objects.size()-1;
}

uint32_t JavaHeap::createObject(ClassInfo* class_info)
{
    Object* object = (Object*) Platform::allocateMemory(sizeof(Array), 0);
    object->type = OBJECT;
    object->fields = 0;
    object->fieldsCount = 0;

    // TODO: Initialize the fields properly and do this resursive

    objects.push_back(object);
    return objects.size()-1;
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

void VM::updateVariableFromOperand(Variable* variable, char* descriptor, StackFrame* frame)
{
    // TODO: implement setting of field data for double and long
    if (strcmp(descriptor, "I") == 0)
    {
        if (variable->type != VariableType_INT)
        {
            fprintf(stderr, "Error: Type mismatch!\n");
            Platform::exitProgram(-78);
        }

        if (frame->operands.size() <= 0)
        {
            fprintf(stderr, "Error: No operands on stack found!\n");
            Platform::exitProgram(-78);
        }

        if (frame->operands[frame->operands.size()-1].type != VariableType_INT)
        {
            fprintf(stderr, "Error: Operand on stack is of the wrong type!\n");
            Platform::exitProgram(-90);
        }
        variable->data = frame->operands[frame->operands.size()-1].data;
        frame->operands.pop_back();
    } else if (descriptor[0] == '[') {

        if (variable->type != VariableType_REFERENCE)
        {
            fprintf(stderr, "Error: Type mismatch!\n");
            Platform::exitProgram(-78);
        }

        if (frame->operands.size() <= 0)
        {
            fprintf(stderr, "Error: No operands on stack found!\n");
            Platform::exitProgram(-78);
        }

        if (frame->operands[frame->operands.size()-1].type != VariableType_REFERENCE)
        {
            fprintf(stderr, "Error: Operand on stack is of the wrong type!\n");
            Platform::exitProgram(-90);
        }
        variable->data = frame->operands[frame->operands.size()-1].data;
        frame->operands.pop_back();
    } else if (descriptor[0] == 'L') {

        if (variable->type != VariableType_REFERENCE)
        {
            fprintf(stderr, "Error: Type mismatch!\n");
            Platform::exitProgram(-78);
        }

        if (frame->operands.size() <= 0)
        {
            fprintf(stderr, "Error: No operands on stack found!\n");
            Platform::exitProgram(-78);
        }

        if (frame->operands[frame->operands.size()-1].type != VariableType_REFERENCE)
        {
            fprintf(stderr, "Error: Operand on stack is of the wrong type!\n");
            Platform::exitProgram(-90);
        }
        variable->data = frame->operands[frame->operands.size()-1].data;
        frame->operands.pop_back();
    } else
    {
        fprintf(stderr, "Error: Setting of variable of type with descriptor: %s not implemented yet!\n", descriptor);
        Platform::exitProgram(-6);
    }
}

void VM::executeLoop()
{
    while(!thread.stack.frames.empty())
    {
        StackFrame* topFrame = &thread.stack.frames[thread.stack.frames.size()-1];
        uint8_t* code = thread.currentMethod->code->code;
        uint8_t opcode = code[thread.pc++];
        printf("Running instruction with opcode: 0x%0x\n", opcode);
        switch (opcode)
        {
        case 0x1: // aconst_null
            {
                Variable reference = {};
                reference.type = VariableType_REFERENCE;
                reference.data = 0;
                topFrame->operands.push_back(reference);
                break;
            }
        case 0x2: //iconst_m1
            {
                Variable variable;
                variable.type = VariableType_INT;
                variable.data = ((int32_t)-1);
                topFrame->operands.push_back(variable);
                break;
            }
        case 0x3: //iconst_0
            {
                Variable variable;
                variable.type = VariableType_INT;
                variable.data = 0;
                topFrame->operands.push_back(variable);
                break;
            }
        case 0x4: //iconst_1
            {
                Variable variable;
                variable.type = VariableType_INT;
                variable.data = ((int32_t)1);
                topFrame->operands.push_back(variable);
                break;
            }
        case 0x5: //iconst_2
            {
                Variable variable;
                variable.type = VariableType_INT;
                variable.data = ((int32_t)2);
                topFrame->operands.push_back(variable);
                break;
            }
        case 0x10: // bipush: push byte
            {

                uint8_t byte = code[thread.pc++];
                Variable variable;
                variable.type = VariableType_INT;
                variable.data = byte;
                topFrame->operands.push_back(variable);
                break;
            }
        case 0x2a: // aload_0
            {
                // TODO: Check if it is OK type and such
                Variable var = topFrame->localVariables[0];
                topFrame->operands.push_back(var);
                break;
            }
        case 0xb3: // putstatic
            {
                uint8_t indexByte1 = code[thread.pc++];
                uint8_t indexByte2 = code[thread.pc++];
                uint16_t index = (indexByte1 << 8) | indexByte2;
                CPFieldRef* fieldRef =  (CPFieldRef*) topFrame->constantPool->constants[index-1];
                CPClassInfo* classInfo =  topFrame->constantPool->getClassInfo(fieldRef->classIndex);
                CPNameAndTypeInfo* nameAndType = (CPNameAndTypeInfo*) topFrame->constantPool->constants[fieldRef->nameAndTypeIndex-1];
                const char* className = topFrame->constantPool->getString(classInfo->nameIndex);
                ClassInfo* targetClass = getClass(className);
                FieldInfo* targetField = targetClass->findField(topFrame->constantPool->getString(nameAndType->nameIndex), topFrame->constantPool->getString(nameAndType->descriptorIndex));
                updateVariableFromOperand(targetField->staticData, topFrame->constantPool->getString(nameAndType->descriptorIndex), topFrame);
                break;
            }
        case 0xb1: // return
            {
                thread.pc = topFrame->previousPc;
                thread.currentClass = topFrame->previousClass;
                thread.currentMethod = topFrame->previousMethod;
                thread.stack.frames.pop_back();
                if (thread.stack.frames.size() > 0)
                {
                    StackFrame* previousStackFrame = &thread.stack.frames[thread.stack.frames.size()-1];
                    thread.currentFrame = previousStackFrame;
                } else
                {
                    thread.currentFrame = 0;
                }
                break;
            }
        case 0x59: // dup
            {
                Variable top = topFrame->operands.back();
                Variable copy = {};
                copy.type = top.type;
                copy.data = top.data;
                topFrame->operands.push_back(copy);
                break;
            }
        case 0xb7: // Invoke special
            {
                uint8_t indexByte1 = code[thread.pc++];
                uint8_t indexByte2 = code[thread.pc++];
                uint16_t index = (indexByte1 << 8) | indexByte2;
                CPMethodRef* methodRef = (CPMethodRef*) topFrame->constantPool->constants[index-1];
                CPClassInfo* cpClassInfo = topFrame->constantPool->getClassInfo(methodRef->classIndex);
                CPNameAndTypeInfo* nameAndTypeInfo = (CPNameAndTypeInfo*) topFrame->constantPool->constants[methodRef->nameAndTypeIndex-1];
                const char* methodName = topFrame->constantPool->getString(nameAndTypeInfo->nameIndex);
                const char* methodDescriptor = topFrame->constantPool->getString(nameAndTypeInfo->descriptorIndex);
                const char* className = topFrame->constantPool->getString(cpClassInfo->nameIndex);
                ClassInfo* targetClassInfo = getClass(className);
                MethodInfo* methodInfo = targetClassInfo->findMethodWithNameAndDescriptor(methodName, methodDescriptor);
                // TODO: Implement argument passing (including subclass argument)
                // TODO: Check correct parsing of descriptors with subclasses

                if (strcmp(methodName, "<init>") ==0)
                {
                    // TODO: Check argument types

                    StackFrame* previousFrame = topFrame;
                    StackFrame stackFrame;
                    stackFrame.localVariables.reserve(methodInfo->code->maxLocals);
                    stackFrame.operands.reserve(methodInfo->code->maxStack);
                    stackFrame.constantPool = targetClassInfo->constantPool;
                    stackFrame.previousPc = thread.pc;
                    stackFrame.previousClass = thread.currentClass;
                    stackFrame.previousMethod = thread.currentMethod;

                    thread.pc = 0;
                    thread.currentClass = targetClassInfo;
                    thread.currentMethod = methodInfo;

                    thread.stack.frames.push_back(stackFrame);
                    thread.currentFrame = &thread.stack.frames[thread.stack.frames.size()-1];

                    for (int i = 0; i <= methodInfo->argsCount; ++i)
                    {
                        Variable var;
                        var.data = 0;
                        var.type = VariableType_UNDEFINED;
                        thread.currentFrame->localVariables.push_back(var);
                    }

                    for (int i = methodInfo->argsCount; i > 0; --i) {
                        thread.currentFrame->localVariables[i] = previousFrame->operands.back();
                        previousFrame->operands.pop_back();
                    }

                    thread.currentFrame->localVariables[0] = previousFrame->operands.back();
                    previousFrame->operands.pop_back();
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
                uint8_t indexByte1 = code[thread.pc++];
                uint8_t indexByte2 = code[thread.pc++];
                uint16_t index = (indexByte1 << 8) | indexByte2;
                CPMethodRef* methodRef = (CPMethodRef*) topFrame->constantPool->constants[index-1];
                CPClassInfo* targetClassInfo = topFrame->constantPool->getClassInfo(methodRef->classIndex);
                CPNameAndTypeInfo* nameAndTypeInfo = (CPNameAndTypeInfo*) topFrame->constantPool->constants[methodRef->nameAndTypeIndex-1];
                ClassInfo* targetClass = getClass(topFrame->constantPool->getString(targetClassInfo->nameIndex));
                // TODO: Take in account descriptor of method as well, for overriding and such
                MethodInfo* methodInfo = targetClass->findMethodWithName(topFrame->constantPool->getString(nameAndTypeInfo->nameIndex));

                if (!methodInfo->isStatic())
                {
                    fprintf(stderr, "Error: Running non-static method as static\n");
                    Platform::exitProgram(-10);
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
        case 0xbb:
            {
                uint8_t indexByte1 = code[thread.pc++];
                uint8_t indexByte2 = code[thread.pc++];
                uint16_t index = (indexByte1 << 8) | indexByte2;

                CPClassInfo* cpClasInfo = topFrame->constantPool->getClassInfo(index);
                ClassInfo* targetClass = getClass(topFrame->constantPool->getString(cpClasInfo->nameIndex));

                uint32_t reference = heap.createObject(targetClass);
                Variable variable;
                variable.type = VariableType_REFERENCE;
                variable.data = reference;

                topFrame->operands.push_back(variable);

                break;
            }
        case 0xbd: // anewarray
            {
                uint8_t indexByte1 = code[thread.pc++];
                uint8_t indexByte2 = code[thread.pc++];
                uint16_t index = (indexByte1 << 8) | indexByte2;

                CPClassInfo* cpclassInfo = topFrame->constantPool->getClassInfo(index);
                ClassInfo* classInfo = getClass(topFrame->constantPool->getString(cpclassInfo->nameIndex));

                int32_t size = topFrame->operands[topFrame->operands.size()-1].data;

                topFrame->operands.pop_back();

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

void VM::runStaticInitializer(ClassInfo* classInfo)
{
    MethodInfo* entryPoint = classInfo->findMethodWithName("<clinit>");

    if (entryPoint == 0)
    {
        // No static initializers for this class
        return;
    }

    StackFrame stackFrame;
    stackFrame.localVariables.reserve(entryPoint->code->maxLocals);
    stackFrame.operands.reserve(entryPoint->code->maxStack);
    stackFrame.constantPool = classInfo->constantPool;
    stackFrame.previousPc = thread.pc;
    stackFrame.previousClass = thread.currentClass;
    stackFrame.previousMethod = thread.currentMethod;


    thread.pc = 0;
    thread.currentClass = classInfo;
    thread.currentMethod = entryPoint;

    thread.stack.frames.push_back(stackFrame);
    thread.currentFrame = &thread.stack.frames[thread.stack.frames.size()-1];

    printf("Executing static initializers...\n");
    executeLoop();
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
        runStaticInitializer(classInfo);
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

    StackFrame stackFrame;
    stackFrame.localVariables.reserve(entryPoint->code->maxLocals);
    stackFrame.operands.reserve(entryPoint->code->maxStack);
    stackFrame.constantPool = startupClass->constantPool;
    stackFrame.previousPc = 0;
    stackFrame.previousClass = 0;
    stackFrame.previousMethod = 0;

    thread.stack.frames.push_back(stackFrame);
    thread.currentFrame = &thread.stack.frames[thread.stack.frames.size()-1];

    printf("Executing main method...\n");
    executeLoop();
    printf("Done executing\n");
}
