#include "VM.h"

#include <stack>

VM::VM()
{
}

void VM::start()
{
    getClass("java/lang/Object");
    getClass("java/lang/String");
}

ClassInfo* VM::getClass(const char* className)
{
    if (!bootstrapClassLoader.isClassLoaded(className)) {
        Memory *memory = new Memory(2000, 1024 * 1024 * 50);
        printf("Loading class %s...\n", className);
        ClassInfo *classInfo = bootstrapClassLoader.readClass(className, memory);
        // TODO: Run static initializers (clinit)
        return classInfo;
    } else {
        return bootstrapClassLoader.getClass(className);
    }
}

void VM::runMain(const char* className)
{
    Memory memory(1000, KIB(5);
    ClassInfo* startupClass = getClass(className);
    MethodInfo* entryPoint = startupClass->findMethodWithName("main");

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
