/*
 * Copyright (c) 2023-2024 Sebastiaan Fernandez.
 *
 * This file is part of VigurVM.
 *
 * VigurVM is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * VigurVM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with Foobar.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "ReferenceInstructions.h"

#include "VM/VM.h"
#include "Data/Variable.h"
#include "VM/Native.h"

#include <bit>
#include <string>

static u2 readShort(VMThread* thread)
{
    uint8_t indexByte1 = thread->m_currentMethod->code->code[thread->m_pc++];
    uint8_t indexByte2 = thread->m_currentMethod->code->code[thread->m_pc++];
    uint16_t shortCombined = (indexByte1 << 8) | indexByte2;
    return shortCombined;
}


[[nodiscard]] static constexpr u2 combineBytes(const u1 byte1, const u1 byte2) {
    return (byte1 << 8) | byte2;
}

void getstatic(INSTRUCTION_ARGS)
{
    uint16_t index = readShort(thread);
    CPFieldRef* fieldRef =  thread->m_currentFrame->constantPool->getFieldRef(index);
    CPClassInfo* classInfo =  thread->m_currentFrame->constantPool->getClassInfo(fieldRef->classIndex);
    CPNameAndTypeInfo* nameAndType = thread->m_currentFrame->constantPool->getNameAndTypeInfo(fieldRef->nameAndTypeIndex);
    const char* className = thread->m_currentFrame->constantPool->getString(classInfo->nameIndex);
    ClassInfo* targetClass = VM->getClass(className, thread);
    FieldInfo* targetField = targetClass->findField(thread->m_currentFrame->constantPool->getString(nameAndType->nameIndex),
        thread->m_currentFrame->constantPool->getString(nameAndType->descriptorIndex));
    const char* descriptor = thread->m_currentFrame->constantPool->getString(nameAndType->descriptorIndex);
    const u1 varCount = VM::getDescriptorVarCategory(descriptor);
    // TODO: Check type
    Variable *vars = targetField->staticData;
    for (u1 currentVar = 0; currentVar < varCount; ++currentVar)
    {
        if (vars[currentVar].type == VariableType_UNDEFINED)
        {
            thread->internalError("Error: Unitialized field not supported in getstatic yet");
        }
        thread->m_currentFrame->operands.push_back(vars[currentVar]);
    }
}

void putstatic(INSTRUCTION_ARGS)
{
    uint16_t index = readShort(thread);
    CPFieldRef* fieldRef =  thread->m_currentFrame->constantPool->getFieldRef(index);
    CPClassInfo* classInfo =  thread->m_currentFrame->constantPool->getClassInfo(fieldRef->classIndex);
    CPNameAndTypeInfo* nameAndType = thread->m_currentFrame->constantPool->getNameAndTypeInfo(fieldRef->nameAndTypeIndex);
    const char* className = thread->m_currentFrame->constantPool->getString(classInfo->nameIndex);
    ClassInfo* targetClass = VM->getClass(className, thread);
    FieldInfo* targetField = targetClass->findField(thread->m_currentFrame->constantPool->getString(nameAndType->nameIndex),
        thread->m_currentFrame->constantPool->getString(nameAndType->descriptorIndex));
    const char* descriptor = thread->m_currentFrame->constantPool->getString(nameAndType->descriptorIndex);
    const u1 varCount = VM::getDescriptorVarCategory(descriptor);
    Variable var = thread->m_currentFrame->popOperand();
    Variable var2{VariableType_UNDEFINED};
    if (varCount == 2)
    {
        var2 = thread->m_currentFrame->popOperand();
    }
    VM->updateVariableFromVariable(targetField->staticData, descriptor, var, var2, thread);
}

void getfield(INSTRUCTION_ARGS)
{
    uint16_t index = readShort(thread);
    CPFieldRef* fieldRef = thread->m_currentFrame->constantPool->getFieldRef(index);
    CPClassInfo* cpClassInfo = thread->m_currentFrame->constantPool->getClassInfo(fieldRef->classIndex);
    CPNameAndTypeInfo* nameAndType = thread->m_currentFrame->constantPool->getNameAndTypeInfo(fieldRef->nameAndTypeIndex);

    const char* className = thread->m_currentFrame->constantPool->getString(cpClassInfo->nameIndex);
    ClassInfo* targetClass = VM->getClass(className, thread);
    const char* fieldName = thread->m_currentFrame->constantPool->getString(nameAndType->nameIndex);
    const char* fieldDescr = thread->m_currentFrame->constantPool->getString(nameAndType->descriptorIndex);


    Variable referencePointer = thread->m_currentFrame->popOperand();

    if (referencePointer.type != VariableType_REFERENCE)
    {
        fprintf(stderr, "Error: Expected object reference on stack\n");
        Platform::exitProgram(6);
    }

    if (referencePointer.data == 0)
    {
        fprintf(stderr, "Error: Nullpointer Exception\n");
        Platform::exitProgram(6);
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
            thread->m_currentFrame->operands.push_back(vars[currentVar]);
        }
    }
}

void putfield(INSTRUCTION_ARGS)
{
    uint16_t index = readShort(thread);
    CPFieldRef* fieldRef = thread->m_currentFrame->constantPool->getFieldRef(index);
    CPClassInfo* cpClassInfo = thread->m_currentFrame->constantPool->getClassInfo(fieldRef->classIndex);
    CPNameAndTypeInfo* nameAndType = thread->m_currentFrame->constantPool->getNameAndTypeInfo(fieldRef->nameAndTypeIndex);

    const char* className = thread->m_currentFrame->constantPool->getString(cpClassInfo->nameIndex);
    ClassInfo* targetClass = VM->getClass(className, thread);
    const char* fieldName = thread->m_currentFrame->constantPool->getString(nameAndType->nameIndex);
    const char* fieldDescr = thread->m_currentFrame->constantPool->getString(nameAndType->descriptorIndex);

    Variable targetValue = thread->m_currentFrame->popOperand();
    Variable targetValue2{VariableType_UNDEFINED};
    if (targetValue.getCategory() == 2)
    {
        targetValue2 = thread->m_currentFrame->popOperand();
    }
    Variable referencePointer = thread->m_currentFrame->popOperand();

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

static void invokeVirtual(ClassInfo* classInfo, MethodInfo* methodInfo, VMThread* thread, VM* VM, JavaHeap* heap) {
    StackFrame* topFrame = thread->m_currentFrame;
    std::deque<Variable> arguments;

    MethodInfo* targetMethod = nullptr;
    ClassInfo* targetClass = nullptr;

    if (topFrame != nullptr)
    {
        // The arguments and the pointer to the object
        for (int i = methodInfo->argsCount; i >= 0; --i)
        {
            arguments.push_front(topFrame->popOperand());
        }

        const Variable ref = arguments[0];
        if (ref.type == VariableType_REFERENCE && ref.data == 0)
        {
            thread->internalError("NullpointerException in virtual call");
        }

        // Look for method based on the object
        Object* object = heap->getObject(ref.data);
        targetClass = object->classInfo;
        MethodInfo* foundMethod = targetClass->findMethodWithNameAndDescriptor(methodInfo->name,
            classInfo->constantPool->getString(methodInfo->descriptorIndex));
        if (foundMethod != nullptr)
        {
            targetMethod = foundMethod;
        } else
        {

            ClassInfo* currentClass = targetClass;
            while (currentClass->superClass != 0) {
                CPClassInfo* ci = currentClass->constantPool->getClassInfo(currentClass->superClass);
                [[maybe_unused]] const char* superClass = currentClass->constantPool->getString(ci->nameIndex);
                currentClass = VM->getClass(superClass, thread);
                if (currentClass != nullptr) {
                    foundMethod = currentClass->findMethodWithNameAndDescriptor(methodInfo->name,
                classInfo->constantPool->getString(methodInfo->descriptorIndex));
                    if (foundMethod != nullptr) {
                        targetClass = currentClass;
                        targetMethod = foundMethod;
                        break;
                    }
                } else {
                    break;
                }
            }

            if (foundMethod == nullptr) {
                thread->internalError("Failed to get the correct method on the object.\n"
                                " Searching on superclass and generic search is not implemented yet.");
            }
        }
    }

    if (methodInfo->isNative()) {
        thread->pushNativeStackFrame(targetClass, targetMethod, arguments.size());
    } else {
        thread->pushStackFrameWithoutParams(targetClass, targetMethod);
    }

    if (!arguments.empty())
    {
        for (int i = 0; i <= methodInfo->argsCount; ++i)
        {
            thread->m_currentFrame->localVariables[i] = arguments[i];
        }
    }

    if (methodInfo->isNative())
    {
        VM->executeNativeMethod(targetClass, targetMethod, heap, thread);
        thread->popFrame();
    }
}

void invokevirtual(INSTRUCTION_ARGS)
{
    StackFrame* topFrame = thread->m_currentFrame;
    const u2 index = readShort(thread);
    const CPMethodRef* methodRef = topFrame->constantPool->getMethodRef(index);
    const CPClassInfo* cpClassInfo = topFrame->constantPool->getClassInfo(methodRef->classIndex);
    const CPNameAndTypeInfo* nameAndTypeInfo = topFrame->constantPool->getNameAndTypeInfo(methodRef->nameAndTypeIndex);
    const char* methodName = topFrame->constantPool->getString(nameAndTypeInfo->nameIndex);
    const char* methodDescriptor = topFrame->constantPool->getString(nameAndTypeInfo->descriptorIndex);
    const char* className = topFrame->constantPool->getString(cpClassInfo->nameIndex);
    ClassInfo* targetClassInfo = VM->getClass(className, thread);
    MethodInfo* methodInfo = targetClassInfo->findMethodWithNameAndDescriptor(methodName, methodDescriptor);

    invokeVirtual(targetClassInfo, methodInfo, thread, VM, heap);
    printf("> Created new stack frame for virtual call on: %s.%s()\n", className, methodName);
}

void invokespecial(INSTRUCTION_ARGS)
{
    StackFrame* topFrame = thread->m_currentFrame;
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
    thread->pushStackFrameSpecial(targetClassInfo, methodInfo, topFrame, heap);
    printf("> Created new stack frame for method call %s on: %s\n", thread->m_currentFrame->methodName, thread->m_currentFrame->className);
}

void invokestatic(INSTRUCTION_ARGS)
{
    StackFrame* topFrame = thread->m_currentFrame;
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
        std::deque<Variable> arguments;
        // The arguments and the pointer to the object
        for (int i = methodInfo->argsCount; i > 0; --i)
        {
            arguments.push_front(topFrame->popOperand());
        }
        thread->pushNativeStackFrame(targetClass, methodInfo, methodInfo->argsCount);

        if (!arguments.empty())
        {
            for (int i = 0; i < methodInfo->argsCount; ++i)
            {
                thread->m_currentFrame->localVariables[i] = arguments[i];
            }
        }


        VM->executeNativeMethod(targetClass, methodInfo, heap, thread);
        thread->popFrame();
    } else
    {
        thread->pushStackFrameStatic(targetClass, methodInfo, topFrame);
        printf("> Created new stack frame for static call: %s.%s\n",
            topFrame->constantPool->getString(targetClassInfo->nameIndex), methodInfo->name);
    }
}

void invokeinterface(INSTRUCTION_ARGS) {
    StackFrame* topFrame = thread->m_currentFrame;
    const u2 index = combineBytes(args[0], args[1]);
    [[maybe_unused]] const u1 count = args[2];
    CPInterfaceRef* interfaceMethodRef =  thread->m_currentClass->constantPool->getInterfaceMethodRef(index);
    CPClassInfo* targetClassInfo = topFrame->constantPool->getClassInfo(interfaceMethodRef->classIndex);
    CPNameAndTypeInfo* nameAndTypeInfo = topFrame->constantPool->getNameAndTypeInfo(interfaceMethodRef->nameAndTypeIndex);
    ClassInfo* targetClass = VM->getClass(topFrame->constantPool->getString(targetClassInfo->nameIndex), thread);
    // TODO: Take in account descriptor of method as well, for overriding and such
    MethodInfo* methodInfo = targetClass->findMethodWithName(topFrame->constantPool->getString(nameAndTypeInfo->nameIndex));

    invokeVirtual(targetClass, methodInfo, thread, VM, heap);
    printf("> Created new stack frame for virtual call on: %s.%s()\n", thread->m_currentFrame->className, thread->m_currentFrame->methodName);
}

void newInstruction(INSTRUCTION_ARGS)
{
    StackFrame* topFrame = thread->m_currentFrame;
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

void newarray(INSTRUCTION_ARGS)
{
    u1 typeArg = args[0];

    ArrayType type = (ArrayType) typeArg;

    const Variable countVar = thread->m_currentFrame->popOperand();
    VM->checkType(countVar, VariableType_INT, thread);

    const uint32_t reference = heap->createArray(type, countVar.data);
    const Variable variable{VariableType_REFERENCE, reference};

    thread->m_currentFrame->operands.push_back(variable);
}

void anewarray(INSTRUCTION_ARGS)
{
    StackFrame* topFrame = thread->m_currentFrame;
    uint16_t index = readShort(thread);
    CPClassInfo* cpclassInfo = topFrame->constantPool->getClassInfo(index);
    [[maybe_unused]] ClassInfo* classInfo = VM->getClass(topFrame->constantPool->getString(cpclassInfo->nameIndex), thread);

    int32_t size = std::bit_cast<i4>(topFrame->popOperand().data);

    if (size < 0)
    {
        thread->internalError("Error: Can't create an array with negative size");
    }

    uint32_t reference = heap->createArray(AT_REFERENCE, size);
    Variable variable{VariableType_REFERENCE, reference};

    topFrame->operands.push_back(variable);
}

void arraylength(INSTRUCTION_ARGS)
{
    const Variable arrayRef = thread->m_currentFrame->popOperand();
    VM->checkType(arrayRef, VariableType_REFERENCE, thread);
    const Array* array = heap->getArray(arrayRef.data);

    const Variable lengthVar{VariableType_INT, static_cast<uint32_t>(array->length)};
    thread->m_currentFrame->operands.push_back(lengthVar);
}

void checkCast(INSTRUCTION_ARGS) {
    // TODO: Implement actual type check
}

void monitorenter(INSTRUCTION_ARGS)
{
    // TODO: Implement when real threading is implemented
    Variable ref = thread->m_currentFrame->popOperand();
    if (ref.type != VariableType_REFERENCE)
    {
        thread->internalError("Expected object reference on stack");
    }

    if (ref.data == 0)
    {
        thread->internalError("Nullpointer Exception");
    }
}

void monitorexit(INSTRUCTION_ARGS)
{
    // TODO: Implement when real threading is implemented
    Variable ref = thread->m_currentFrame->popOperand();
    if (ref.type != VariableType_REFERENCE)
    {
        fprintf(stderr, "Error: Expected object reference on stack\n");
        Platform::exitProgram(6);
    }

    if (ref.data == 0) {
        thread->internalError("Nullpointer Exception");
    }
}
