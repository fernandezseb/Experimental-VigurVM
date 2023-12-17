#include "ReferenceInstructions.h"

#include "VM.h"
#include "Data/Variable.h"

static u2 readShort(VMThread* thread)
{
    uint8_t indexByte1 = thread->currentMethod->code->code[thread->pc++];
    uint8_t indexByte2 = thread->currentMethod->code->code[thread->pc++];
    uint16_t shortCombined = (indexByte1 << 8) | indexByte2;
    return shortCombined;
}

void getstatic(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    uint16_t index = readShort(thread);
    CPFieldRef* fieldRef =  thread->currentFrame->constantPool->getFieldRef(index);
    CPClassInfo* classInfo =  thread->currentFrame->constantPool->getClassInfo(fieldRef->classIndex);
    CPNameAndTypeInfo* nameAndType = thread->currentFrame->constantPool->getNameAndTypeInfo(fieldRef->nameAndTypeIndex);
    const char* className = thread->currentFrame->constantPool->getString(classInfo->nameIndex);
    ClassInfo* targetClass = VM->getClass(className, thread);
    FieldInfo* targetField = targetClass->findField(thread->currentFrame->constantPool->getString(nameAndType->nameIndex),
        thread->currentFrame->constantPool->getString(nameAndType->descriptorIndex));
    const char* descriptor = thread->currentFrame->constantPool->getString(nameAndType->descriptorIndex);
    u2 varCount = VM->getDescriptorVarCount((char*)descriptor);
    if (varCount != 1)
    {
        fprintf(stderr, "Error: getstatic not implemented for double length values\n");
        Platform::exitProgram(-56);
    }
    // TODO: Check type, this can also be unitialized
    Variable *var = targetField->staticData;
    if (var->type == VariableType_UNDEFINED)
    {
        fprintf(stderr, "Error: Unitialized field not supported in getstatic yet\n");
        Platform::exitProgram(-56);
    }
    thread->currentFrame->operands.push_back(*var);
}

void putstatic(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{uint16_t index = readShort(thread);
    CPFieldRef* fieldRef =  thread->currentFrame->constantPool->getFieldRef(index);
    CPClassInfo* classInfo =  thread->currentFrame->constantPool->getClassInfo(fieldRef->classIndex);
    CPNameAndTypeInfo* nameAndType = thread->currentFrame->constantPool->getNameAndTypeInfo(fieldRef->nameAndTypeIndex);
    const char* className = thread->currentFrame->constantPool->getString(classInfo->nameIndex);
    ClassInfo* targetClass = VM->getClass(className, thread);
    FieldInfo* targetField = targetClass->findField(thread->currentFrame->constantPool->getString(nameAndType->nameIndex),
        thread->currentFrame->constantPool->getString(nameAndType->descriptorIndex));
    Variable var = thread->currentFrame->popOperand();
    VM->updateVariableFromVariable(targetField->staticData, thread->currentFrame->constantPool->getString(nameAndType->descriptorIndex), var);
}

void putfield(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    uint16_t index = readShort(thread);
    CPFieldRef* fieldRef = thread->currentFrame->constantPool->getFieldRef(index);
    CPClassInfo* cpClassInfo = thread->currentFrame->constantPool->getClassInfo(fieldRef->classIndex);
    CPNameAndTypeInfo* nameAndType = thread->currentFrame->constantPool->getNameAndTypeInfo(fieldRef->nameAndTypeIndex);

    const char* className = thread->currentFrame->constantPool->getString(cpClassInfo->nameIndex);
    ClassInfo* targetClass = VM->getClass(className, thread);
    const char* fieldName = thread->currentFrame->constantPool->getString(nameAndType->nameIndex);
    const char* fieldDescr = thread->currentFrame->constantPool->getString(nameAndType->descriptorIndex);


    // TODO: Get the object from the reference -> Done
    // TODO: Get the correct field from the object -> Done
    // TODO: Update the value of the object -> Basic Done

    Variable targetValue = thread->currentFrame->popOperand();
    Variable referencePointer = thread->currentFrame->popOperand();

    Object* targetObject = heap->getChildObject(referencePointer.data, targetClass);

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
}

void invokespecial(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    StackFrame* topFrame = thread->currentFrame;
    uint16_t index = readShort(thread);
    CPMethodRef* methodRef = topFrame->constantPool->getMethodRef(index);
    CPClassInfo* cpClassInfo = topFrame->constantPool->getClassInfo(methodRef->classIndex);
    CPNameAndTypeInfo* nameAndTypeInfo = topFrame->constantPool->getNameAndTypeInfo(methodRef->nameAndTypeIndex);
    const char* methodName = topFrame->constantPool->getString(nameAndTypeInfo->nameIndex);
    const char* methodDescriptor = topFrame->constantPool->getString(nameAndTypeInfo->descriptorIndex);
    const char* className = topFrame->constantPool->getString(cpClassInfo->nameIndex);
    ClassInfo* targetClassInfo = VM->getClass(className, thread);
    MethodInfo* methodInfo = targetClassInfo->findMethodWithNameAndDescriptor(methodName, methodDescriptor);
    // TODO: Implement argument passing (including subclass argument)
    // TODO: Check correct parsing of descriptors with subclasses

    if (strcmp(methodName, "<init>") ==0)
    {
        // TODO: Check argument types
        VM->pushStackFrameVirtual(targetClassInfo, methodInfo, topFrame, thread);

        printf("> Created new stack frame for constructor call on: %s\n", className);
    } else
    {
        fprintf(stderr, "Error: Invokespecial not implemented for other cases than constructors\n");
        Platform::exitProgram(-30);
    }

}

void invokestatic(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    StackFrame* topFrame = thread->currentFrame;
    uint16_t index = readShort(thread);
    CPMethodRef* methodRef = topFrame->constantPool->getMethodRef(index);
    CPClassInfo* targetClassInfo = topFrame->constantPool->getClassInfo(methodRef->classIndex);
    CPNameAndTypeInfo* nameAndTypeInfo = topFrame->constantPool->getNameAndTypeInfo(methodRef->nameAndTypeIndex);
    ClassInfo* targetClass = VM->getClass(topFrame->constantPool->getString(targetClassInfo->nameIndex), thread);
    // TODO: Take in account descriptor of method as well, for overriding and such
    MethodInfo* methodInfo = targetClass->findMethodWithName(topFrame->constantPool->getString(nameAndTypeInfo->nameIndex));

    if (!methodInfo->isStatic())
    {
        // TODO: Do method call by pushing stack frame
        fprintf(stderr, "Error: Running non-static method as static\n");
        Platform::exitProgram(-10);
    }

    if (methodInfo->isNative())
    {
        printf("Running native code of method: %s\n", methodInfo->name);
    } else
    {
        VM->pushStackFrameStatic(targetClass, methodInfo, topFrame, thread);
        printf("> Created new stack frame for constructor call on: %s\n",
            topFrame->constantPool->getString(targetClassInfo->nameIndex));
    }
}

void newInstruction(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    StackFrame* topFrame = thread->currentFrame;
    uint16_t index = readShort(thread);
    CPClassInfo* cpClasInfo = topFrame->constantPool->getClassInfo(index);
    ClassInfo* targetClass = VM->getClass(topFrame->constantPool->getString(cpClasInfo->nameIndex), thread);

    const char* superClassName = targetClass->constantPool->getString(
    targetClass->constantPool->getClassInfo(targetClass->superClass)->nameIndex);
    while (strcmp(superClassName, "java/lang/Object") != 0)
    {
        ClassInfo* superClass = VM->getClass(superClassName, thread);
        superClassName = superClass->constantPool->getString(
            superClass->constantPool->getClassInfo(superClass->superClass)->nameIndex);
    }

    uint32_t reference = heap->createObject(targetClass);
    Variable variable = {};
    variable.type = VariableType_REFERENCE;
    variable.data = reference;

    topFrame->operands.push_back(variable);
}

void anewarray(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    StackFrame* topFrame = thread->currentFrame;
    uint16_t index = readShort(thread);
    CPClassInfo* cpclassInfo = topFrame->constantPool->getClassInfo(index);
    ClassInfo* classInfo = VM->getClass(topFrame->constantPool->getString(cpclassInfo->nameIndex), thread);

    int32_t size = topFrame->popOperand().data;

    if (size < 0)
    {
        fprintf(stderr, "Error: Can't create an array with negative size\n");
        Platform::exitProgram(-6);
    }

    uint32_t reference = heap->createArray(AT_REFERENCE, size);
    Variable variable;
    variable.type = VariableType_REFERENCE;
    variable.data = reference;

    topFrame->operands.push_back(variable);
}
