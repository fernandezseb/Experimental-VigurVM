#include "ReferenceInstructions.h"

#include "VM/VM.h"
#include "Data/Variable.h"
#include "VM/Native.h"

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
    const u1 varCount = VM::getDescriptorVarCategory(descriptor);
    // TODO: Check type
    Variable *vars = targetField->staticData;
    for (u1 currentVar = 0; currentVar < varCount; ++currentVar)
    {
        if (vars[currentVar].type == VariableType_UNDEFINED)
        {
            thread->internalError("Error: Unitialized field not supported in getstatic yet");
        }
        thread->currentFrame->operands.push_back(vars[currentVar]);
    }
}

void putstatic(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
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
    const u1 varCount = VM::getDescriptorVarCategory(descriptor);
    Variable var = thread->currentFrame->popOperand();
    Variable var2{VariableType_UNDEFINED};
    if (varCount == 2)
    {
        var2 = thread->currentFrame->popOperand();
    }
    VM->updateVariableFromVariable(targetField->staticData, descriptor, var, var2, thread);
}

void getfield(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    uint16_t index = readShort(thread);
    CPFieldRef* fieldRef = thread->currentFrame->constantPool->getFieldRef(index);
    CPClassInfo* cpClassInfo = thread->currentFrame->constantPool->getClassInfo(fieldRef->classIndex);
    CPNameAndTypeInfo* nameAndType = thread->currentFrame->constantPool->getNameAndTypeInfo(fieldRef->nameAndTypeIndex);

    const char* className = thread->currentFrame->constantPool->getString(cpClassInfo->nameIndex);
    ClassInfo* targetClass = VM->getClass(className, thread);
    const char* fieldName = thread->currentFrame->constantPool->getString(nameAndType->nameIndex);
    const char* fieldDescr = thread->currentFrame->constantPool->getString(nameAndType->descriptorIndex);


    Variable referencePointer = thread->currentFrame->popOperand();

    if (referencePointer.type != VariableType_REFERENCE)
    {
        fprintf(stderr, "Error: Expected object reference on stack\n");
        Platform::exitProgram(-6);
    }

    if (referencePointer.data == 0)
    {
        fprintf(stderr, "Error: Nullpointer Exception\n");
        Platform::exitProgram(-6);
    }
    else
    {
        Object* object = heap->getChildObject(referencePointer.data, targetClass);
        FieldData* field = object->getField(fieldName, fieldDescr, heap);
        if (field == 0)
        {
            thread->internalError("Error: Field not found in object!");
        }
        Variable* vars = field->data;
        for (u1 currentVar = 0; currentVar < field->dataSize; ++currentVar)
        {
            thread->currentFrame->operands.push_back(vars[currentVar]);
        }
    }
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

    Variable targetValue = thread->currentFrame->popOperand();
    Variable targetValue2{VariableType_UNDEFINED};
    if (targetValue.getCategory() == 2)
    {
        targetValue2 = thread->currentFrame->popOperand();
    }
    Variable referencePointer = thread->currentFrame->popOperand();

    Object* targetObject = heap->getChildObject(referencePointer.data, targetClass);

    FieldData* fieldData = targetObject->getField(fieldName, fieldDescr, heap);

    // TODO: Check for type descriptor
    fieldData->data[0] = targetValue;
    if (targetValue.getCategory() == 2 && fieldData->dataSize == 2)
    {
        // ARRAY: | MOST SIGN | LEAST SIGN |
        fieldData->data[0] = targetValue2; // Most significant bits
        fieldData->data[1] = targetValue;  // Least significant bits
    }
}

void invokevirtual(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    StackFrame* topFrame = thread->currentFrame;
    u2 index = readShort(thread);
    CPMethodRef* methodRef = topFrame->constantPool->getMethodRef(index);
    CPClassInfo* cpClassInfo = topFrame->constantPool->getClassInfo(methodRef->classIndex);
    CPNameAndTypeInfo* nameAndTypeInfo = topFrame->constantPool->getNameAndTypeInfo(methodRef->nameAndTypeIndex);
    const char* methodName = topFrame->constantPool->getString(nameAndTypeInfo->nameIndex);
    const char* methodDescriptor = topFrame->constantPool->getString(nameAndTypeInfo->descriptorIndex);
    const char* className = topFrame->constantPool->getString(cpClassInfo->nameIndex);
    ClassInfo* targetClassInfo = VM->getClass(className, thread);
    MethodInfo* methodInfo = targetClassInfo->findMethodWithNameAndDescriptor(methodName, methodDescriptor);
    thread->pushStackFrameVirtual(targetClassInfo, methodInfo, topFrame);
    printf("> Created new stack frame for virtual call on: %s.%s()\n", className, methodName);
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

    // TODO: Check argument types
    thread->pushStackFrameVirtual(targetClassInfo, methodInfo, topFrame);
    printf("> Created new stack frame for method call %s on: %s\n", methodName, className);

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
        thread->internalError("Error: Running non-static method as static");
    }

    if (methodInfo->isNative())
    {
        const char* className = targetClass->getName();
        printf("Running native code of method: %s\n", methodInfo->name);
        const char* description = topFrame->constantPool->getString(nameAndTypeInfo->descriptorIndex);
        const char* methodName = methodInfo->name;
        char fullName[200] = {0};
        strcat(fullName, className);
        strcat(fullName, "/");
        strcat(fullName, methodName);
        nativeImplementation impl = findNativeMethod(fullName, description);
        if (impl != nullptr)
        {
            impl(heap, thread, VM);
        } else
        {
            char errorString[400];
            snprintf(errorString, 400, "Can't find native method %s%s", fullName, description);
            thread->internalError(errorString);
        }
    } else
    {
        thread->pushStackFrameStatic(targetClass, methodInfo, topFrame);
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

    // Check if we are not in java/lang/Object, because that class doesn't have a superClas
    if (targetClass->superClass != 0)
    {
        const char* superClassName = targetClass->constantPool->getString(
        targetClass->constantPool->getClassInfo(targetClass->superClass)->nameIndex);
        while (strcmp(superClassName, "java/lang/Object") != 0)
        {
            ClassInfo* superClass = VM->getClass(superClassName, thread);
            superClassName = superClass->constantPool->getString(
                superClass->constantPool->getClassInfo(superClass->superClass)->nameIndex);
        }
    }

    const uint32_t reference = heap->createObject(targetClass, VM);
    const Variable variable{VariableType_REFERENCE, reference};

    topFrame->operands.push_back(variable);
}

void newarray(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    u1 typeArg = args[0];

    ArrayType type = (ArrayType) typeArg;

    const Variable countVar = thread->currentFrame->popOperand();
    VM->checkType(countVar, VariableType_INT, thread);

    const uint32_t reference = heap->createArray(type, countVar.data);
    const Variable variable{VariableType_REFERENCE, reference};

    thread->currentFrame->operands.push_back(variable);
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
        thread->internalError("Error: Can't create an array with negative size");
    }

    uint32_t reference = heap->createArray(AT_REFERENCE, size);
    Variable variable{VariableType_REFERENCE, reference};

    topFrame->operands.push_back(variable);
}

void arraylength(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    const Variable arrayRef = thread->currentFrame->popOperand();
    VM->checkType(arrayRef, VariableType_REFERENCE, thread);
    const Array* array = heap->getArray(arrayRef.data);

    const Variable lengthVar{VariableType_INT, static_cast<uint32_t>(array->length)};
    thread->currentFrame->operands.push_back(lengthVar);
}

void monitorenter(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    // TODO: Implement when real threading is implemented
    Variable ref = thread->currentFrame->popOperand();
    if (ref.type != VariableType_REFERENCE)
    {
        thread->internalError("Expected object reference on stack");
    }

    if (ref.data == 0)
    {
        thread->internalError("Nullpointer Exception");
    }
}

void monitorexit(uint8_t* args, uint16_t argsCount, int8_t arg, JavaHeap* heap, VMThread* thread, VM* VM)
{
    // TODO: Implement when real threading is implemented
    Variable ref = thread->currentFrame->popOperand();
    if (ref.type != VariableType_REFERENCE)
    {
        fprintf(stderr, "Error: Expected object reference on stack\n");
        Platform::exitProgram(-6);
    }

    if (ref.data == 0) {
        thread->internalError("Nullpointer Exception");
    }
}
