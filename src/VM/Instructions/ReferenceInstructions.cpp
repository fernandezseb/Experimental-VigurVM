/*
 * Copyright (c) 2023-2025 Sebastiaan Fernandez.
 *
 * This file is part of VigurVM.
 *
 * VigurVM is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * VigurVM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with VigurVM.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "ReferenceInstructions.h"

#include "VM/VM.h"
#include "Data/VData.h"
#include "VM/Native.h"

#include <bit>
#include <deque>
#include <string>
#include <thread>


[[nodiscard]] static constexpr u2 combineBytes(const u1 byte1, const u1 byte2) {
    return (byte1 << 8) | byte2;
}

void getstatic(const InstructionInput& input)
{
    const u2 index = input.thread->readSignedShort();
    const CPFieldRef* fieldRef =  input.thread->m_currentFrame->constantPool->getFieldRef(index);
    const CPClassInfo* classInfo =  input.thread->m_currentFrame->constantPool->getClassInfo(fieldRef->classIndex);
    const CPNameAndTypeInfo* nameAndType = input.thread->m_currentFrame->constantPool->getNameAndTypeInfo(fieldRef->nameAndTypeIndex);
    const std::string_view className = input.thread->m_currentFrame->constantPool->getString(classInfo->nameIndex);
    ClassInfo* targetClass = input.thread->getClass(className.data());
    const std::string_view fieldName = input.thread->m_currentFrame->constantPool->getString(nameAndType->nameIndex);
    const FieldInfo* targetField = input.thread->findField(targetClass,
        fieldName.data(),
        input.thread->m_currentFrame->constantPool->getString(nameAndType->descriptorIndex).data());
    const std::string_view descriptor = input.thread->m_currentFrame->constantPool->getString(nameAndType->descriptorIndex);
    const u1 varCount = getDescriptorVarCategory(descriptor);
    // TODO: Check type
    const vdata var = targetField->staticData;
    if (var.type == VariableType_UNDEFINED)
    {
        input.thread->internalError("Error: Unitialized field not supported in getstatic yet");
    }
    input.thread->m_currentFrame->operands.push_back(var);
    if (var.getCategory() == 2) {
        input.thread->m_currentFrame->operands.push_back(var);
    }
}

void putstatic(const InstructionInput& input)
{
    const uint16_t index = input.thread->readSignedShort();
    CPFieldRef* fieldRef =  input.thread->m_currentFrame->constantPool->getFieldRef(index);
    CPClassInfo* classInfo =  input.thread->m_currentFrame->constantPool->getClassInfo(fieldRef->classIndex);
    CPNameAndTypeInfo* nameAndType = input.thread->m_currentFrame->constantPool->getNameAndTypeInfo(fieldRef->nameAndTypeIndex);
    const std::string_view className = input.thread->m_currentFrame->constantPool->getString(classInfo->nameIndex);
    ClassInfo* targetClass = input.thread->getClass(className.data());
    FieldInfo* targetField = input.thread->findField(targetClass,
        input.thread->m_currentFrame->constantPool->getString(nameAndType->nameIndex).data(),
        input.thread->m_currentFrame->constantPool->getString(nameAndType->descriptorIndex).data());
    const std::string_view descriptor = input.thread->m_currentFrame->constantPool->getString(nameAndType->descriptorIndex);
    const u1 varCount = getDescriptorVarCategory(descriptor);
    // With cat 2 vars, var should be the LSB
    vdata var = input.thread->m_currentFrame->popOperand();
    vdata var2{VariableType_UNDEFINED};
    if (varCount == 2)
    {
        var2 = input.thread->m_currentFrame->popOperand();
    }
    VariableType type = fromDescriptor(descriptor.data());
    targetField->staticData.checkType(type);
    var.checkType(type);
    targetField->staticData.value = var.value;
}

void getfield(const InstructionInput& input)
{
    uint16_t index = input.thread->readSignedShort();
    CPFieldRef* fieldRef = input.thread->m_currentFrame->constantPool->getFieldRef(index);
    CPClassInfo* cpClassInfo = input.thread->m_currentFrame->constantPool->getClassInfo(fieldRef->classIndex);
    CPNameAndTypeInfo* nameAndType = input.thread->m_currentFrame->constantPool->getNameAndTypeInfo(fieldRef->nameAndTypeIndex);

    const std::string_view className = input.thread->m_currentFrame->constantPool->getString(cpClassInfo->nameIndex);
    ClassInfo* targetClass = input.thread->getClass(className.data());
    const std::string_view fieldName = input.thread->m_currentFrame->constantPool->getString(nameAndType->nameIndex);
    const std::string_view fieldDescr = input.thread->m_currentFrame->constantPool->getString(nameAndType->descriptorIndex);


    vdata referencePointer = input.thread->m_currentFrame->popOperand();

    if (referencePointer.type != VariableType_REFERENCE)
    {
        fprintf(stderr, "Error: Expected object reference on stack\n");
        Platform::exitProgram(6);
    }

    if (referencePointer.getReference() == 0)
    {
        fprintf(stderr, "Error: Nullpointer Exception\n");
        Platform::exitProgram(6);
    }
    else
    {
        const Object* object = VM::get()->getHeap()->getChildObject(referencePointer.getReference(), targetClass);
        FieldData* field = object->getField(fieldName.data(), fieldDescr.data());
        if (field == 0)
        {
            input.thread->internalError("Error: Field not found in object!");
        }
        input.thread->m_currentFrame->operands.emplace_back(field->type, field->value);
        if (field->dataSize == 2)
        {
            input.thread->m_currentFrame->operands.emplace_back(field->type, field->value);
        }
    }
}

void putfield(const InstructionInput& input)
{
    uint16_t index = input.thread->readSignedShort();
    CPFieldRef* fieldRef = input.thread->m_currentFrame->constantPool->getFieldRef(index);
    CPClassInfo* cpClassInfo = input.thread->m_currentFrame->constantPool->getClassInfo(fieldRef->classIndex);
    CPNameAndTypeInfo* nameAndType = input.thread->m_currentFrame->constantPool->getNameAndTypeInfo(fieldRef->nameAndTypeIndex);

    const std::string_view className = input.thread->m_currentFrame->constantPool->getString(cpClassInfo->nameIndex);
    ClassInfo* targetClass = input.thread->getClass(className.data());
    const std::string_view fieldName = input.thread->m_currentFrame->constantPool->getString(nameAndType->nameIndex);
    const std::string_view fieldDescr = input.thread->m_currentFrame->constantPool->getString(nameAndType->descriptorIndex);

    vdata targetValue = input.thread->m_currentFrame->popOperand();
    vdata targetValue2{VariableType_UNDEFINED};
    if (targetValue.getCategory() == 2)
    {
        targetValue2 = input.thread->m_currentFrame->popOperand(); // MSB
    }
    vdata referencePointer = input.thread->m_currentFrame->popOperand();

    const Object* targetObject = VM::get()->getHeap()->getChildObject(referencePointer.getReference(), targetClass);

    FieldData* fieldData = targetObject->getField(fieldName.data(), fieldDescr.data());

    // TODO: Check for type descriptor
    fieldData->value = targetValue.value;
}

static void invokeVirtual(ClassInfo* classInfo, MethodInfo* methodInfo, VMThread* thread, VM* VM) {
    StackFrame* topFrame = thread->m_currentFrame;
    std::deque<vdata> arguments;

    MethodInfo* targetMethod = nullptr;
    ClassInfo* targetClass = nullptr;

    if (topFrame != nullptr)
    {
        // The arguments and the pointer to the object
        for (int i = methodInfo->argsCount; i >= 0; --i)
        {
            vdata operand = topFrame->popOperand();
            if (operand.getCategory() == 2)
            {
                vdata highByte = topFrame->popOperand();
                arguments.push_front(operand);
                arguments.push_front(highByte);
            } else {
                arguments.push_front(operand);
            }
        }

        const vdata ref = arguments[0];
        if (ref.getReference() == 0)
        {
            thread->internalError("NullpointerException in virtual call");
        }

        // Look for method based on the object
        const Reference* reference = VM::get()->getHeap()->getReference(ref.getReference());
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
                    currentClass = thread->getClass(superClass.data());
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
        thread->executeNativeMethod(targetClass, targetMethod);
        thread->popFrame();
    }
}

void invokevirtual(const InstructionInput& input)
{
    StackFrame* topFrame = input.thread->m_currentFrame;
    const u2 index = input.thread->readSignedShort();
    const CPMethodRef* methodRef = topFrame->constantPool->getMethodRef(index);
    const CPClassInfo* cpClassInfo = topFrame->constantPool->getClassInfo(methodRef->classIndex);
    const CPNameAndTypeInfo* nameAndTypeInfo = topFrame->constantPool->getNameAndTypeInfo(methodRef->nameAndTypeIndex);
    const std::string_view methodName = topFrame->constantPool->getString(nameAndTypeInfo->nameIndex);
    const std::string_view methodDescriptor = topFrame->constantPool->getString(nameAndTypeInfo->descriptorIndex);
    const std::string_view className = topFrame->constantPool->getString(cpClassInfo->nameIndex);
    ClassInfo* targetClassInfo = nullptr;
    if (className.starts_with("["))
    {
        targetClassInfo = input.thread->getClass("java/lang/Object");

    } else {
        targetClassInfo = input.thread->getClass(className.data());
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
            currentClass = input.thread->getClass(superClass.data());
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

    invokeVirtual(targetClassInfo, targetMethod, input.thread, VM::get());
    // printf("> Created new stack frame for virtual call on: %s.%s()\n", className.data(), methodName.data());
}

void invokespecial(const InstructionInput& input)
{
    StackFrame* topFrame = input.thread->m_currentFrame;
    const uint16_t index = input.thread->readSignedShort();
    const CPMethodRef* methodRef = topFrame->constantPool->getMethodRef(index);
    const CPClassInfo* cpClassInfo = topFrame->constantPool->getClassInfo(methodRef->classIndex);
    const CPNameAndTypeInfo* nameAndTypeInfo = topFrame->constantPool->getNameAndTypeInfo(methodRef->nameAndTypeIndex);
    const std::string_view methodName = topFrame->constantPool->getString(nameAndTypeInfo->nameIndex);
    const std::string_view methodDescriptor = topFrame->constantPool->getString(nameAndTypeInfo->descriptorIndex);
    const std::string_view className = topFrame->constantPool->getString(cpClassInfo->nameIndex);
    ClassInfo* targetClassInfo = input.thread->getClass(className.data());
    const MethodInfo* methodInfo = targetClassInfo->findMethodWithNameAndDescriptor(methodName.data(), methodDescriptor.data());
    // TODO: Implement argument passing (including subclass argument)
    // TODO: Check correct parsing of descriptors with subclasses

    // TODO: Check argument types
    input.thread->pushStackFrameSpecial(targetClassInfo, methodInfo, topFrame);

    if (methodInfo->isNative())
    {
        input.thread->executeNativeMethod(targetClassInfo, methodInfo);
        input.thread->popFrame();
    }

    // printf("> Created new stack frame for method call %s on: %s\n", input.thread->m_currentFrame->methodName.data(), input.thread->m_currentFrame->className.data());
}

void invokestatic(const InstructionInput& input)
{
    StackFrame* topFrame = input.thread->m_currentFrame;
    const uint16_t index = input.thread->readSignedShort();
    CPMethodRef* methodRef = topFrame->constantPool->getMethodRef(index);
    CPClassInfo* targetClassInfo = topFrame->constantPool->getClassInfo(methodRef->classIndex);
    CPNameAndTypeInfo* nameAndTypeInfo = topFrame->constantPool->getNameAndTypeInfo(methodRef->nameAndTypeIndex);
    ClassInfo* targetClass = input.thread->getClass(topFrame->constantPool->getString(targetClassInfo->nameIndex).data());
    MethodInfo* methodInfo = targetClass->findMethodWithNameAndDescriptor(
        topFrame->constantPool->getString(nameAndTypeInfo->nameIndex).data(),
        topFrame->constantPool->getString(nameAndTypeInfo->descriptorIndex).data());

    if (!methodInfo->isStatic())
    {
        input.thread->internalError("Error: Running non-static method as static");
    }

    if (methodInfo->isNative())
    {
        std::deque<vdata> arguments;
        // The arguments and the pointer to the object
        for (int i = methodInfo->argsCount; i > 0; --i)
        {
            vdata operand = topFrame->popOperand();
            if (operand.getCategory() == 2)
            {
                vdata highByte = topFrame->popOperand();
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

        input.thread->executeNativeMethod(targetClass, methodInfo);
        input.thread->popFrame();
    } else
    {
        input.thread->pushStackFrameStatic(targetClass, methodInfo, topFrame);
        // printf("> Created new stack frame for static call: %s.%s\n",
        // topFrame->constantPool->getString(targetClassInfo->nameIndex).data(), methodInfo->name.data());
    }
}

void invokeinterface(const InstructionInput& input) {
    StackFrame* topFrame = input.thread->m_currentFrame;
    const u2 index = combineBytes(input.args[0], input.args[1]);
    [[maybe_unused]] const u1 count = input.args[2];
    CPInterfaceRef* interfaceMethodRef =  input.thread->m_currentClass->constantPool->getInterfaceMethodRef(index);
    CPClassInfo* targetClassInfo = topFrame->constantPool->getClassInfo(interfaceMethodRef->classIndex);
    CPNameAndTypeInfo* nameAndTypeInfo = topFrame->constantPool->getNameAndTypeInfo(interfaceMethodRef->nameAndTypeIndex);
    ClassInfo* targetClass = input.thread->getClass(topFrame->constantPool->getString(targetClassInfo->nameIndex).data());
    MethodInfo* methodInfo = targetClass->findMethodWithNameAndDescriptor(
        topFrame->constantPool->getString(nameAndTypeInfo->nameIndex).data(),
        topFrame->constantPool->getString(nameAndTypeInfo->descriptorIndex).data());

    invokeVirtual(targetClass, methodInfo, input.thread, VM::get());
    // printf("> Created new stack frame for virtual call on: %s.%s()\n", input.thread->m_currentFrame->className.data(), input.thread->m_currentFrame->methodName.data());
}

void newInstruction(const InstructionInput& input)
{
    StackFrame* topFrame = input.thread->m_currentFrame;
    const uint16_t index = input.thread->readSignedShort();
    CPClassInfo* cpClasInfo = topFrame->constantPool->getClassInfo(index);
    ClassInfo* targetClass = input.thread->getClass(topFrame->constantPool->getString(cpClasInfo->nameIndex).data());

    // Check if we are not in java/lang/Object, because that class doesn't have a superClas
    if (targetClass->superClass != 0)
    {
        std::string_view superClassName = targetClass->constantPool->getString(
        targetClass->constantPool->getClassInfo(targetClass->superClass)->nameIndex);
        while (superClassName != "java/lang/Object")
        {
            ClassInfo* superClass = input.thread->getClass(superClassName.data());
            superClassName = superClass->constantPool->getString(
                superClass->constantPool->getClassInfo(superClass->superClass)->nameIndex);
        }
    }

    const vreference reference = VM::get()->getHeap()->createObject(targetClass);
    const vdata variable{VariableType_REFERENCE, reference};

    topFrame->operands.push_back(variable);
}

void newarray(const InstructionInput& input)
{
    const u1 typeArg = input.args[0];
    const ArrayType type = (ArrayType) typeArg;

    const vdata countVar = input.thread->m_currentFrame->popOperand();

    // TODO: Fix by putting the correct descriptor based on the type
    const vreference reference = VM::get()->getHeap()->createArray(type, countVar.getInt(), "");
    const vdata variable{VariableType_REFERENCE, reference};

    input.thread->m_currentFrame->operands.push_back(variable);
}

void anewarray(const InstructionInput& input)
{
    StackFrame* topFrame = input.thread->m_currentFrame;
    u2 index = input.thread->readSignedShort();
    CPClassInfo* cpclassInfo = topFrame->constantPool->getClassInfo(index);
    const char* className = topFrame->constantPool->getString(cpclassInfo->nameIndex).data();
    // TODO: Is this needed? This loads the class eagerly
    [[maybe_unused]] ClassInfo* classInfo = input.thread->getClass(className);

    const vint size = topFrame->popOperand().getInt();

    if (size < 0)
    {
        input.thread->internalError("Error: Can't create an array with negative size");
    }

    std::string str = std::string(className);
    str.insert(0, "L");
    str.push_back(';');
    const uint32_t reference = VM::get()->getHeap()->createArray(AT_REFERENCE, size, str);
    const vdata variable{VariableType_REFERENCE, reference};

    topFrame->operands.push_back(variable);
}

void arraylength(const InstructionInput& input)
{
    const vdata arrayRef = input.thread->m_currentFrame->popOperand();
    const Array* array = VM::get()->getHeap()->getArray(arrayRef.getReference());

    const vdata lengthVar{VariableType_INT, static_cast<vint>(array->length)};
    input.thread->m_currentFrame->operands.push_back(lengthVar);
}

void athrow(const InstructionInput& input)
{
    const vdata throwableRef = input.thread->m_currentFrame->popOperand();
    const Object* throwable = VM::get()->getHeap()->getObject(throwableRef.getReference());

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
                const ClassInfo* catchTypeClass = input.thread->getClass(catchTypeName);
                if (input.thread->isSubclass(catchTypeClass, throwable->classInfo))
                {
                    input.thread->m_pc = handler.handlerPc;
                    input.thread->m_currentFrame->operands.clear();
                    input.thread->m_currentFrame->pushObject(throwableRef.getReference());
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
    printf("String pool:\n");
    VM::get()->getHeap()->printStringPool();
    input.thread->internalError("Unhandled exception");
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

    const vdata operand = input.thread->m_currentFrame->popOperand();
    operand.checkType(VariableType_REFERENCE);

    i4 returnVal = 0;

    if (operand.getReference() == 0)
    {
        input.thread->m_currentFrame->pushInt(returnVal);
        return;
    }

    const Reference* reference = VM::get()->getHeap()->getReference(operand.getReference());

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

            const ClassInfo* targetClassInfo = input.thread->getClass(name.data());
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
                    object = VM::get()->getHeap()->getObject(object->superClassObject);
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
    const vdata ref = input.thread->m_currentFrame->popOperand();

    if (ref.getReference() == 0)
    {
        input.thread->internalError("Nullpointer Exception");
    }
}

void monitorexit(const InstructionInput& input)
{
    // TODO: Implement when real threading is implemented
    const vdata ref = input.thread->m_currentFrame->popOperand();

    if (ref.getReference() == 0) {
        input.thread->internalError("Nullpointer Exception");
    }
}
