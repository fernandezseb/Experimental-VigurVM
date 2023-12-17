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
