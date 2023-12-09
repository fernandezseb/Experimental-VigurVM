#include "VM.h"

#include "../Memory.h"

VM::VM()
{
}

void VM::start()
{
    // getClass("java/lang/Object");
    // getClass("java/lang/String");
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

ClassInfo* VM::getClass(const char* className)
{
    ClassInfo* classInfo = getClassByName(className);
    if (classInfo == NULL) {
        Memory *memory = new Memory(2000, MIB(20));
        printf("Loading class %s...\n", className);
        ClassInfo *classInfo = bootstrapClassLoader.readClass(className, memory);
        // TODO: Run static initializers (clinit)
        heap.methodArea.classes.add(classInfo);
        return classInfo;
    }
    return classInfo;
}

void VM::runMain(const char* className)
{
    Memory memory(1000, KIB(5));
    ClassInfo* startupClass = getClass(className);
    MethodInfo* entryPoint = startupClass->findMethodWithName("main");
    if (entryPoint == 0)
    {
        fprintf(stderr, "Error: Entry point not found. Exiting...\n");
    }

    thread.pc = 0;
    thread.currentClass = startupClass;
    thread.currentMethod = entryPoint;
    thread.name = "main";
    thread.stack.frames.reserve(200);

    StackFrame stackFrame;
    stackFrame.localVariables.reserve(entryPoint->code->maxLocals);
    stackFrame.operands.reserve(entryPoint->code->maxStack);
    stackFrame.constantPool = startupClass->constantPool;
    stackFrame.previousPc = 0;

    thread.stack.frames.push_back(stackFrame);
    thread.currentFrame = &thread.stack.frames[0];

    printf("brol");
}
