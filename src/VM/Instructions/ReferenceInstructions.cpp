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
#include <deque>
#include <string>
#include <thread>

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

void getstatic(const InstructionInput& input)
{
    const uint16_t index = readShort(input.thread);
    const CPFieldRef* fieldRef =  input.thread->m_currentFrame->constantPool->getFieldRef(index);
    const CPClassInfo* classInfo =  input.thread->m_currentFrame->constantPool->getClassInfo(fieldRef->classIndex);
    const CPNameAndTypeInfo* nameAndType = input.thread->m_currentFrame->constantPool->getNameAndTypeInfo(fieldRef->nameAndTypeIndex);
    const std::string_view className = input.thread->m_currentFrame->constantPool->getString(classInfo->nameIndex);
    ClassInfo* targetClass = input.vm->getClass(className.data(), input.thread);
    const std::string_view fieldName = input.thread->m_currentFrame->constantPool->getString(nameAndType->nameIndex);
    const FieldInfo* targetField = input.vm->findField(targetClass,
        fieldName.data(),
        input.thread->m_currentFrame->constantPool->getString(nameAndType->descriptorIndex).data(),
        input.thread);
    const std::string_view descriptor = input.thread->m_currentFrame->constantPool->getString(nameAndType->descriptorIndex);
    const u1 varCount = VM::getDescriptorVarCategory(descriptor);
    // TODO: Check type
    const Variable *vars = targetField->staticData;
    for (u1 currentVar = 0; currentVar < varCount; ++currentVar)
    {
        if (vars[currentVar].type == VariableType_UNDEFINED)
        {
            input.thread->internalError("Error: Unitialized field not supported in getstatic yet");
        }
        input.thread->m_currentFrame->operands.push_back(vars[currentVar]);
    }
}

void putstatic(const InstructionInput& input)
{
    uint16_t index = readShort(input.thread);
    CPFieldRef* fieldRef =  input.thread->m_currentFrame->constantPool->getFieldRef(index);
    CPClassInfo* classInfo =  input.thread->m_currentFrame->constantPool->getClassInfo(fieldRef->classIndex);
    CPNameAndTypeInfo* nameAndType = input.thread->m_currentFrame->constantPool->getNameAndTypeInfo(fieldRef->nameAndTypeIndex);
    const std::string_view className = input.thread->m_currentFrame->constantPool->getString(classInfo->nameIndex);
    ClassInfo* targetClass = input.vm->getClass(className.data(), input.thread);
    FieldInfo* targetField = input.vm->findField(targetClass,
        input.thread->m_currentFrame->constantPool->getString(nameAndType->nameIndex).data(),
        input.thread->m_currentFrame->constantPool->getString(nameAndType->descriptorIndex).data(),
        input.thread);
    const std::string_view descriptor = input.thread->m_currentFrame->constantPool->getString(nameAndType->descriptorIndex);
    const u1 varCount = VM::getDescriptorVarCategory(descriptor);
    // With cat 2 vars, var should be the LSB
    Variable var = input.thread->m_currentFrame->popOperand();
    Variable var2{VariableType_UNDEFINED};
    if (varCount == 2)
    {
        var2 = input.thread->m_currentFrame->popOperand();
    }
    input.vm->updateVariableFromVariable(targetField->staticData, descriptor.data(), var, var2, input.thread);
}

void getfield(const InstructionInput& input)
{
    uint16_t index = readShort(input.thread);
    CPFieldRef* fieldRef = input.thread->m_currentFrame->constantPool->getFieldRef(index);
    CPClassInfo* cpClassInfo = input.thread->m_currentFrame->constantPool->getClassInfo(fieldRef->classIndex);
    CPNameAndTypeInfo* nameAndType = input.thread->m_currentFrame->constantPool->getNameAndTypeInfo(fieldRef->nameAndTypeIndex);

    const std::string_view className = input.thread->m_currentFrame->constantPool->getString(cpClassInfo->nameIndex);
    ClassInfo* targetClass = input.vm->getClass(className.data(), input.thread);
    const std::string_view fieldName = input.thread->m_currentFrame->constantPool->getString(nameAndType->nameIndex);
    const std::string_view fieldDescr = input.thread->m_currentFrame->constantPool->getString(nameAndType->descriptorIndex);


    Variable referencePointer = input.thread->m_currentFrame->popOperand();

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
        const Object* object = input.heap->getChildObject(referencePointer.data, targetClass);
        FieldData* field = object->getField(fieldName.data(), fieldDescr.data(), input.heap);
        if (field == 0)
        {
            input.thread->internalError("Error: Field not found in object!");
        }
        Variable* vars = field->data;
        for (u1 currentVar = 0; currentVar < field->dataSize; ++currentVar)
        {
            input.thread->m_currentFrame->operands.push_back(vars[currentVar]);
        }
    }
}

void putfield(const InstructionInput& input)
{
    uint16_t index = readShort(input.thread);
    CPFieldRef* fieldRef = input.thread->m_currentFrame->constantPool->getFieldRef(index);
    CPClassInfo* cpClassInfo = input.thread->m_currentFrame->constantPool->getClassInfo(fieldRef->classIndex);
    CPNameAndTypeInfo* nameAndType = input.thread->m_currentFrame->constantPool->getNameAndTypeInfo(fieldRef->nameAndTypeIndex);

    const std::string_view className = input.thread->m_currentFrame->constantPool->getString(cpClassInfo->nameIndex);
    ClassInfo* targetClass = input.vm->getClass(className.data(), input.thread);
    const std::string_view fieldName = input.thread->m_currentFrame->constantPool->getString(nameAndType->nameIndex);
    const std::string_view fieldDescr = input.thread->m_currentFrame->constantPool->getString(nameAndType->descriptorIndex);

    Variable targetValue = input.thread->m_currentFrame->popOperand();
    Variable targetValue2{VariableType_UNDEFINED};
    if (targetValue.getCategory() == 2)
    {
        targetValue2 = input.thread->m_currentFrame->popOperand(); // MSB
    }
    Variable referencePointer = input.thread->m_currentFrame->popOperand();

    const Object* targetObject = input.heap->getChildObject(referencePointer.data, targetClass);

    FieldData* fieldData = targetObject->getField(fieldName.data(), fieldDescr.data(), input.heap);

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
            Variable operand = topFrame->popOperand();
            if (operand.getCategory() == 2)
            {
                Variable highByte = topFrame->popOperand();
                arguments.push_front(operand);
                arguments.push_front(highByte);
            } else {
                arguments.push_front(operand);
            }
        }

        const Variable ref = arguments[0];
        if (ref.type == VariableType_REFERENCE && ref.data == 0)
        {
            thread->internalError("NullpointerException in virtual call");
        }

        // Look for method based on the object
        const Reference* reference = heap->getReference(ref.data);
        if (reference->type == OBJECT || reference->type == CLASSOBJECT) {
            const Object* object = reference->getObject();
            targetClass = object->classInfo;
            MethodInfo* foundMethod = targetClass->findMethodWithNameAndDescriptor(methodInfo->name.data(),
                classInfo->constantPool->getString(methodInfo->descriptorIndex).data());
            if (foundMethod != nullptr)
            {
                targetMethod = foundMethod;
            } else
            {

                ClassInfo* currentClass = targetClass;
                while (currentClass->superClass != 0) {
                    CPClassInfo* ci = currentClass->constantPool->getClassInfo(currentClass->superClass);
                    [[maybe_unused]] const std::string_view superClass = currentClass->constantPool->getString(ci->nameIndex);
                    currentClass = VM->getClass(superClass.data(), thread);
                    if (currentClass != nullptr) {
                        foundMethod = currentClass->findMethodWithNameAndDescriptor(methodInfo->name.data(),
                    classInfo->constantPool->getString(methodInfo->descriptorIndex).data());
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
        } else
        {
            // Execute java.lang.Object method on array
            targetClass = classInfo;
            targetMethod = methodInfo;
            printf("");
        }
    }

    if (targetMethod->isNative()) {
        thread->pushNativeStackFrame(targetClass, targetMethod, arguments.size());
    } else {
        thread->pushStackFrameWithoutParams(targetClass, targetMethod);
    }

    if (!arguments.empty())
    {
        for (int i = 0; i < arguments.size(); ++i)
        {
            thread->m_currentFrame->localVariables[i] = arguments[i];
        }
    }

    if (targetMethod->isNative())
    {
        VM->executeNativeMethod(targetClass, targetMethod, heap, thread);
        thread->popFrame();
    }
}

void invokevirtual(const InstructionInput& input)
{
    StackFrame* topFrame = input.thread->m_currentFrame;
    const u2 index = readShort(input.thread);
    const CPMethodRef* methodRef = topFrame->constantPool->getMethodRef(index);
    const CPClassInfo* cpClassInfo = topFrame->constantPool->getClassInfo(methodRef->classIndex);
    const CPNameAndTypeInfo* nameAndTypeInfo = topFrame->constantPool->getNameAndTypeInfo(methodRef->nameAndTypeIndex);
    const std::string_view methodName = topFrame->constantPool->getString(nameAndTypeInfo->nameIndex);
    const std::string_view methodDescriptor = topFrame->constantPool->getString(nameAndTypeInfo->descriptorIndex);
    const std::string_view className = topFrame->constantPool->getString(cpClassInfo->nameIndex);
    ClassInfo* targetClassInfo = nullptr;
    if (className.starts_with("["))
    {
        targetClassInfo = input.vm->getClass("java/lang/Object", input.thread);

    } else {
        targetClassInfo = input.vm->getClass(className.data(), input.thread);
    }
    if (targetClassInfo == nullptr)
    {
        printf("Class not found");
    }
    MethodInfo* foundMethod = targetClassInfo->findMethodWithNameAndDescriptor(methodName.data(), methodDescriptor.data());

    MethodInfo* targetMethod = nullptr;




    // BEGIN BROL
    if (foundMethod != nullptr)
    {
        targetMethod = foundMethod;
    } else
    {

        ClassInfo* currentClass = targetClassInfo;
        while (currentClass->superClass != 0) {
            CPClassInfo* ci = currentClass->constantPool->getClassInfo(currentClass->superClass);
            [[maybe_unused]] const std::string_view superClass = currentClass->constantPool->getString(ci->nameIndex);
            currentClass = input.vm->getClass(superClass.data(), input.thread);
            if (currentClass != nullptr) {
                foundMethod = currentClass->findMethodWithNameAndDescriptor(methodName.data(),
            methodDescriptor.data());
                if (foundMethod != nullptr) {
                    targetClassInfo = currentClass;
                    targetMethod = foundMethod;
                    break;
                }
            } else {
                break;
            }
        }

        if (foundMethod == nullptr) {
            input.thread->internalError("Failed to get the correct method on the object.\n"
                            " Searching on superclass and generic search is not implemented yet.");
        }
    }
    // END BROL

    invokeVirtual(targetClassInfo, targetMethod, input.thread, input.vm, input.heap);
    printf("> Created new stack frame for virtual call on: %s.%s()\n", className.data(), methodName.data());
}

void invokespecial(const InstructionInput& input)
{
    StackFrame* topFrame = input.thread->m_currentFrame;
    const uint16_t index = readShort(input.thread);
    const CPMethodRef* methodRef = topFrame->constantPool->getMethodRef(index);
    const CPClassInfo* cpClassInfo = topFrame->constantPool->getClassInfo(methodRef->classIndex);
    const CPNameAndTypeInfo* nameAndTypeInfo = topFrame->constantPool->getNameAndTypeInfo(methodRef->nameAndTypeIndex);
    const std::string_view methodName = topFrame->constantPool->getString(nameAndTypeInfo->nameIndex);
    const std::string_view methodDescriptor = topFrame->constantPool->getString(nameAndTypeInfo->descriptorIndex);
    const std::string_view className = topFrame->constantPool->getString(cpClassInfo->nameIndex);
    ClassInfo* targetClassInfo = input.vm->getClass(className.data(), input.thread);
    const MethodInfo* methodInfo = targetClassInfo->findMethodWithNameAndDescriptor(methodName.data(), methodDescriptor.data());
    // TODO: Implement argument passing (including subclass argument)
    // TODO: Check correct parsing of descriptors with subclasses

    // TODO: Check argument types
    input.thread->pushStackFrameSpecial(targetClassInfo, methodInfo, topFrame, input.heap);

    if (methodInfo->isNative())
    {
        input.vm->executeNativeMethod(targetClassInfo, methodInfo, input.heap, input.thread);
        input.thread->popFrame();
    }

    printf("> Created new stack frame for method call %s on: %s\n", input.thread->m_currentFrame->methodName.data(), input.thread->m_currentFrame->className.data());
}

void invokestatic(const InstructionInput& input)
{
    StackFrame* topFrame = input.thread->m_currentFrame;
    uint16_t index = readShort(input.thread);
    CPMethodRef* methodRef = topFrame->constantPool->getMethodRef(index);
    CPClassInfo* targetClassInfo = topFrame->constantPool->getClassInfo(methodRef->classIndex);
    CPNameAndTypeInfo* nameAndTypeInfo = topFrame->constantPool->getNameAndTypeInfo(methodRef->nameAndTypeIndex);
    ClassInfo* targetClass = input.vm->getClass(topFrame->constantPool->getString(targetClassInfo->nameIndex).data(), input.thread);
    MethodInfo* methodInfo = targetClass->findMethodWithNameAndDescriptor(
        topFrame->constantPool->getString(nameAndTypeInfo->nameIndex).data(),
        topFrame->constantPool->getString(nameAndTypeInfo->descriptorIndex).data());

    if (!methodInfo->isStatic())
    {
        input.thread->internalError("Error: Running non-static method as static");
    }

    if (methodInfo->isNative())
    {
        std::deque<Variable> arguments;
        // The arguments and the pointer to the object
        for (int i = methodInfo->argsCount; i > 0; --i)
        {
            Variable operand = topFrame->popOperand();
            if (operand.getCategory() == 2)
            {
                Variable highByte = topFrame->popOperand();
                arguments.push_front(operand);
                arguments.push_front(highByte);
            } else
            {
                arguments.push_front(operand);
            }
        }
        input.thread->pushNativeStackFrame(targetClass, methodInfo, arguments.size());

        if (!arguments.empty())
        {
            for (int i = 0; i < arguments.size(); ++i)
            {
                input.thread->m_currentFrame->localVariables[i] = arguments[i];
            }
        }

        input.vm->executeNativeMethod(targetClass, methodInfo, input.heap, input.thread);
        input.thread->popFrame();
    } else
    {
        input.thread->pushStackFrameStatic(targetClass, methodInfo, topFrame);
        printf("> Created new stack frame for static call: %s.%s\n",
            topFrame->constantPool->getString(targetClassInfo->nameIndex).data(), methodInfo->name.data());
    }
}

void invokeinterface(const InstructionInput& input) {
    StackFrame* topFrame = input.thread->m_currentFrame;
    const u2 index = combineBytes(input.args[0], input.args[1]);
    [[maybe_unused]] const u1 count = input.args[2];
    CPInterfaceRef* interfaceMethodRef =  input.thread->m_currentClass->constantPool->getInterfaceMethodRef(index);
    CPClassInfo* targetClassInfo = topFrame->constantPool->getClassInfo(interfaceMethodRef->classIndex);
    CPNameAndTypeInfo* nameAndTypeInfo = topFrame->constantPool->getNameAndTypeInfo(interfaceMethodRef->nameAndTypeIndex);
    ClassInfo* targetClass = input.vm->getClass(topFrame->constantPool->getString(targetClassInfo->nameIndex).data(), input.thread);
    MethodInfo* methodInfo = targetClass->findMethodWithNameAndDescriptor(
        topFrame->constantPool->getString(nameAndTypeInfo->nameIndex).data(),
        topFrame->constantPool->getString(nameAndTypeInfo->descriptorIndex).data());

    invokeVirtual(targetClass, methodInfo, input.thread, input.vm, input.heap);
    printf("> Created new stack frame for virtual call on: %s.%s()\n", input.thread->m_currentFrame->className.data(), input.thread->m_currentFrame->methodName.data());
}

void newInstruction(const InstructionInput& input)
{
    StackFrame* topFrame = input.thread->m_currentFrame;
    uint16_t index = readShort(input.thread);
    CPClassInfo* cpClasInfo = topFrame->constantPool->getClassInfo(index);
    ClassInfo* targetClass = input.vm->getClass(topFrame->constantPool->getString(cpClasInfo->nameIndex).data(), input.thread);

    // Check if we are not in java/lang/Object, because that class doesn't have a superClas
    if (targetClass->superClass != 0)
    {
        std::string_view superClassName = targetClass->constantPool->getString(
        targetClass->constantPool->getClassInfo(targetClass->superClass)->nameIndex);
        while (superClassName != "java/lang/Object")
        {
            ClassInfo* superClass = input.vm->getClass(superClassName.data(), input.thread);
            superClassName = superClass->constantPool->getString(
                superClass->constantPool->getClassInfo(superClass->superClass)->nameIndex);
        }
    }

    const uint32_t reference = input.heap->createObject(targetClass, input.vm);
    const Variable variable{VariableType_REFERENCE, reference};

    topFrame->operands.push_back(variable);
}

void newarray(const InstructionInput& input)
{
    const u1 typeArg = input.args[0];
    const ArrayType type = (ArrayType) typeArg;

    const Variable countVar = input.thread->m_currentFrame->popOperand();
    input.vm->checkType(countVar, VariableType_INT, input.thread);

    // TODO: Fix by putting the correct descriptor based on the type
    const uint32_t reference = input.heap->createArray(type, countVar.data, "");
    const Variable variable{VariableType_REFERENCE, reference};

    input.thread->m_currentFrame->operands.push_back(variable);
}

void anewarray(const InstructionInput& input)
{
    StackFrame* topFrame = input.thread->m_currentFrame;
    uint16_t index = readShort(input.thread);
    CPClassInfo* cpclassInfo = topFrame->constantPool->getClassInfo(index);
    const char* className = topFrame->constantPool->getString(cpclassInfo->nameIndex).data();
    // TODO: Is this needed? This loads the class eagerly
    [[maybe_unused]] ClassInfo* classInfo = input.vm->getClass(className, input.thread);

    const int32_t size = std::bit_cast<i4>(topFrame->popOperand().data);

    if (size < 0)
    {
        input.thread->internalError("Error: Can't create an array with negative size");
    }

    std::string *str = new std::string(className);
    str->insert(0, "L");
    str->push_back(';');
    const uint32_t reference = input.heap->createArray(AT_REFERENCE, size, *str);
    const Variable variable{VariableType_REFERENCE, reference};

    topFrame->operands.push_back(variable);
}

void arraylength(const InstructionInput& input)
{
    const Variable arrayRef = input.thread->m_currentFrame->popOperand();
    input.vm->checkType(arrayRef, VariableType_REFERENCE, input.thread);
    const Array* array = input.heap->getArray(arrayRef.data);

    const Variable lengthVar{VariableType_INT, static_cast<uint32_t>(array->length)};
    input.thread->m_currentFrame->operands.push_back(lengthVar);
}

void athrow(const InstructionInput& input)
{
    const Variable throwableRef = input.thread->m_currentFrame->popOperand();
    const Object* throwable = input.heap->getObject(throwableRef.data);

    // remove ==

    printf("String pool:\n");

    input.heap->printStringPool();

    input.thread->internalError("Unhandled exception");

    // ==

    while(!input.thread->m_stack.frames.empty()) {
        const AttributeCode* codeAttribute = input.thread->m_currentMethod->code;
        if (codeAttribute == nullptr)
        {
            // This happens when we have a native code stackframe.
            // In this case we have to skip the frame because it can't and won't contain exception handlers.
            input.thread->popFrame();
            continue;
        }
        const std::span<ExceptionTableEntry> exceptionHandlers =  codeAttribute->exceptionTable;

        for (const ExceptionTableEntry& handler : exceptionHandlers)
        {
            if (input.thread->m_pc >= handler.startPc && input.thread->m_pc <= handler.endPc)
            {
                const CPClassInfo* catchType = input.thread->m_currentClass->constantPool->getClassInfo(handler.catchType);
                const std::string_view catchTypeName = input.thread->m_currentClass->constantPool->getString(catchType->nameIndex);
                const ClassInfo* catchTypeClass = input.vm->getClass(catchTypeName, input.thread);
                if (input.vm->isSubclass(input.thread, catchTypeClass, throwable->classInfo))
                {
                    input.thread->m_pc = handler.handlerPc;
                    input.thread->m_currentFrame->operands.clear();
                    input.thread->m_currentFrame->pushObject(throwableRef.data);
                    return;
                }
            }
        }

        input.thread->popFrame();
    }

    // TODO: Notify other threads when program crashes

    printf("Exception in thread \"%s\" %s\n",
    input.thread->m_name.data(),
    throwable->classInfo->getName().data());
    Platform::exitProgram(1);
    printf("");
}

void checkCast(const InstructionInput& input) {
    // TODO: Implement actual type check
}

void instanceof(const InstructionInput& input)
{
    const u2 index = combineBytes(input.args[0], input.args[1]);
    const CPClassInfo* cpClassInfo =  input.thread->m_currentClass->constantPool->getClassInfo(index);
    const std::string_view name =  input.thread->m_currentClass->constantPool->getString(cpClassInfo->nameIndex);

    const Variable operand = input.thread->m_currentFrame->popOperand();
    input.vm->checkType(operand, VariableType_REFERENCE, input.thread);

    i4 returnVal = 0;

    if (operand.data == 0)
    {
        input.thread->m_currentFrame->pushInt(returnVal);
        return;
    }

    const Reference* reference = input.heap->getReference(operand.data);

    if (reference->type == OBJECT || reference->type == CLASSOBJECT)
    {
        const Object* object = reference->getObject();
        const ClassInfo* classInfo = object->classInfo;
        if (classInfo->isInterface())
        {
            input.thread->internalError("Instanceof not implemented yet for interfaces");
        } else
        {
            std::string_view objectClassName = classInfo->getName();

            const ClassInfo* targetClassInfo = input.vm->getClass(name.data(), input.thread);
            if (targetClassInfo->isInterface())
            {
                for (const uint16_t interfaceIndex : classInfo->interfaces)
                {
                    const CPClassInfo* interfaceInfo = classInfo->constantPool->getClassInfo(interfaceIndex);
                    const std::string_view interfaceName = classInfo->constantPool->getString(interfaceInfo->nameIndex);
                    if (interfaceName == name)
                    {
                        returnVal = 1;
                        break;
                    }
                }
                input.thread->m_currentFrame->pushInt(returnVal);
            } else
                // Check if it is a subclass
            {
                do
                {
                    if (name == objectClassName)
                    {
                        returnVal = 1;
                        break;
                    }
                    if (object->superClassObject == 0)
                    {
                        break;
                    }
                    object = input.heap->getObject(object->superClassObject);
                    if (object != nullptr)
                    {
                        objectClassName = object->classInfo->getName();
                    }
                } while(object != nullptr);
                input.thread->m_currentFrame->pushInt(returnVal);
            }
        }
    } else
    {
        input.thread->internalError("Instanceof not implemented yet for arrays");
    }
}

void monitorenter(const InstructionInput& input)
{
    // TODO: Implement when real threading is implemented
    const Variable ref = input.thread->m_currentFrame->popOperand();
    if (ref.type != VariableType_REFERENCE)
    {
        input.thread->internalError("Expected object reference on stack");
    }

    if (ref.data == 0)
    {
        input.thread->internalError("Nullpointer Exception");
    }
}

void monitorexit(const InstructionInput& input)
{
    // TODO: Implement when real threading is implemented
    const Variable ref = input.thread->m_currentFrame->popOperand();
    if (ref.type != VariableType_REFERENCE)
    {
        fprintf(stderr, "Error: Expected object reference on stack\n");
        Platform::exitProgram(6);
    }

    if (ref.data == 0) {
        input.thread->internalError("Nullpointer Exception");
    }
}
