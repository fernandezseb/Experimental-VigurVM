#pragma once

#include "Core.h"
#include "JavaHeap.h"
#include "JavaStack.h"
#include "Data/Class.h"
#include "Instruction.h"

#include <array>
#include <thread>

class VMThread
{
public:
    u4 m_pc{0};
    JavaStack m_stack;
    // Current frame
    StackFrame* m_currentFrame{nullptr};
    // Current method
    const MethodInfo* m_currentMethod{nullptr};
    // current class
    ClassInfo* m_currentClass{nullptr};
    std::string_view m_name;
    u4 threadObject{0};
    i1 priority{5};
    bool alive{false};
    std::thread::id m_pthreadId;

    explicit VMThread(const std::string_view name, const size_t frameSize) noexcept
        : m_stack(frameSize), m_name(name)
    {
        m_pthreadId = std::this_thread::get_id();
    }
    ClassInfo* getClass(std::string_view className);
    void executeNativeMethod(const ClassInfo* targetClass, const MethodInfo* methodInfo);
    bool isSubclass(const ClassInfo* targetClass, ClassInfo* subClass);
    FieldInfo* findField(ClassInfo* classInfo, const char* name, const char* descriptor);
    void pushStackFrameWithoutParams(ClassInfo* classInfo, const MethodInfo* methodInfo);
    void pushNativeStackFrame(ClassInfo* classInfo, const MethodInfo* methodInfo, size_t argumentsSize);
    void popFrame();
    void pushStackFrameStatic(ClassInfo* classInfo, MethodInfo* methodInfo, StackFrame* previousFrame);
    void pushStackFrameSpecial(ClassInfo* classInfo, const MethodInfo* methodInfo, StackFrame* previousFrame);
    void returnVar(Variable returnValue);
    void returnVar(Variable highByte, Variable lowByte);
    void internalError(std::string_view error, i4 errorCode) const;
    void internalError(std::string_view error) const;
    u1 readUnsignedByte();
    u2 readUnsignedShort();
    i4 readSignedInt();

    StackFrame* getTopFrameNonNative();
    void executeLoop();
private:
    void initStaticFields(ClassInfo* class_info);
    void runStaticInitializer(ClassInfo* classInfo);
};
